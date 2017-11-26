#ifndef SFDAY_COMMON_THREAD_POOL_INCLUDE_THREAD_POOL_H_
#define SFDAY_COMMON_THREAD_POOL_INCLUDE_THREAD_POOL_H_

#include <stdint.h>
#include <pthread.h>
#include <vector>
#include <queue>

namespace sfday {

class Task {
    public:
        Task();
        Task(void (*callback)(void *), void *args);
        ~Task();

        void set_task(void (*callback)(void *), void *args);

    private:
        friend class ThreadPool;
        pthread_mutex_t *m_mutex;
        pthread_cond_t  *m_cond;
        bool            *m_is_done;

        void (*m_callback)(void *);
        void *m_args;
};

class ThreadPool {
    public:
        ThreadPool();
        ~ThreadPool();

        int run_sync(Task &new_task);
        int run_async(Task &new_task);

        void set_maximum_waiting_tasks(const uint64_t max);
        void set_maximum_workers(const uint64_t max);

        int start();
        void stop();

    private:
        pthread_mutex_t        m_mutex;
        pthread_cond_t         m_cond;

        std::vector<pthread_t> m_workers;
        std::queue<Task>       m_tasks;

        uint64_t m_maximum_waiting_tasks;
        uint64_t m_maximum_workers;

        bool m_is_working;

        static void *worker(void *self);
};

}
#endif

