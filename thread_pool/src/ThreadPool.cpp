#include "common/include/ThreadPool.h"
#include "common/include/errno.h"

namespace sfday {
ThreadPool::ThreadPool()
    : m_workers(),
      m_tasks(),
      m_maximum_waiting_tasks(65536),
      m_maximum_workers(32),
      m_is_working(false) {
    pthread_mutex_init(&m_mutex, 0);
    pthread_cond_init(&m_cond, 0);
}

ThreadPool::~ThreadPool() {
    stop();
    pthread_cond_destroy(&m_cond);
    pthread_mutex_destroy(&m_mutex);
}

void ThreadPool::set_maximum_waiting_tasks(const uint64_t max) {
    m_maximum_waiting_tasks = max;
}

void ThreadPool::set_maximum_workers(const uint64_t max) {
    m_maximum_workers = max;
}

void *ThreadPool::worker(void *self) {
    pthread_detach(pthread_self());
    ThreadPool *t_self = (ThreadPool *)self;

    while (1) {
        pthread_mutex_lock(&(t_self->m_mutex));

        while (t_self->m_tasks.size() == 0 && t_self->m_is_working) {
            pthread_cond_wait(&(t_self->m_cond), &(t_self->m_mutex));
        }

        if (t_self->m_tasks.size() == 0 && !(t_self->m_is_working)) {
            pthread_mutex_unlock(&t_self->m_mutex);
            break;
        }

        Task curr = t_self->m_tasks.front();
        t_self->m_tasks.pop();

        pthread_mutex_unlock(&(t_self->m_mutex));

        curr.m_callback(curr.m_args);

        if (NULL != curr.m_mutex && NULL != curr.m_cond && NULL != curr.m_is_done) {
            pthread_mutex_lock(curr.m_mutex);
            *(curr.m_is_done) = true;
            pthread_cond_signal(curr.m_cond);
            pthread_mutex_unlock(curr.m_mutex);
        }
    }

    return NULL;
}

int ThreadPool::run_sync(Task& new_task) {
    int ret = SUCCESS;
    
    pthread_mutex_t sync_mutex;
    pthread_cond_t  sync_cond;
    bool is_done = false;

    pthread_mutex_init(&sync_mutex, 0);
    pthread_cond_init(&sync_cond, 0);
    new_task.m_mutex   = &sync_mutex;
    new_task.m_cond    = &sync_cond;
    new_task.m_is_done = &is_done;

    ret = run_async(new_task);
    pthread_mutex_lock(&sync_mutex);
    while (!is_done) {
        pthread_cond_wait(&sync_cond, &sync_mutex);
    }
    pthread_mutex_unlock(&sync_mutex);

    new_task.m_is_done = NULL;
    new_task.m_cond    = NULL;
    new_task.m_mutex   = NULL;
    pthread_cond_destroy(&sync_cond);
    pthread_mutex_destroy(&sync_mutex);

    return ret;
}

int ThreadPool::run_async(Task& new_task) {
    int ret = SUCCESS;

    pthread_mutex_lock(&m_mutex);

    do {
        if (!m_is_working) {
            ret = ERR_THREAD_POOL_NOT_INUSE;
            break;
        }

        if (m_tasks.size() == m_maximum_waiting_tasks) {
            ret = ERR_THREAD_POOL_FULL;
            break;
        }

        m_tasks.push(new_task);
        pthread_cond_broadcast(&m_cond);
    } while (0);

    pthread_mutex_unlock(&m_mutex);

    return SUCCESS;
}

int ThreadPool::start() {
    int ret = SUCCESS;

    pthread_mutex_lock(&m_mutex);
    do {
        if (m_is_working) {
            break;
        }

        m_is_working = true;
        for (int i = 0; i < m_maximum_workers; ++i) {
            pthread_t tid = 0;
            int t_ret = pthread_create(&tid, 0, ThreadPool::worker, this);

            if (0 != t_ret) {
                m_is_working = false;
                ret = ERR_PTHREAD_CREATE_FAILED;
                break;
            }

            m_workers.push_back(tid);
        }

    } while (0);

    pthread_mutex_unlock(&m_mutex);

    return ret;
}

void ThreadPool::stop() {
    pthread_mutex_lock(&m_mutex);
    if (m_is_working) {
        m_is_working = false;
        pthread_cond_broadcast(&m_cond);
    }
    pthread_mutex_unlock(&m_mutex);
}

Task::Task()
    : m_callback(NULL),
      m_args(NULL),
      m_mutex(NULL),
      m_cond(NULL),
      m_is_done(NULL) {}

Task::Task(void (*callback)(void *), void *args)
    : m_callback(callback),
      m_args(args),
      m_mutex(NULL),
      m_cond(NULL),
      m_is_done(NULL) {}

Task::~Task() {}

void Task::set_task(void (*callback)(void *), void *args) {
    m_callback = callback;
    m_args     = args;
}

}

