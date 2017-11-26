/*******************************************************************************
 *
 * Descrition:
 * 
 *     [Descriptions]
 *
 * Author:
 *
 *     Lu Xu (luxu_0519@126.com)
 *
 ******************************************************************************/

#ifndef SFDAY_API_FPGADEVICE_H_
#define SFDAY_API_FPGADEVICE_H_

#include <string>
#include <vector>
#include <unordered_map>
#include "common/include/comm.h"
#include "common/include/errno.h"
#include "common/thread_pool/ThreadPool.h"

namespace sfday {

class FpgaPort {
    public:
        FpgaPort();
        FpgaPort(const uint64_t addr);
        FpgaPort(const uint64_t addr, const std::string& name);
        ~FpgaPort();

        int set_name(const std::string& name);
        int set_addr(const uint64_t addr);
        int set_value(const uint64_t value);

        uint64_t get_value() const;

        std::string name() const;
        std::string type_string() const;
        std::string debug_string() const;

    private:
        uint64_t m_addr;
        uint64_t m_value;
        std::string m_name;
};

class FpgaFunction {
    public:
        FpgaFunction();
        ~FpgaFunction();

        int add_port_args(const uint64_t addr);
        int add_port_args(const uint64_t addr, const std::string& name);
        int add_port_cmd(const uint64_t addr);
        int add_port_signal(const uint64_t addr);

        int set_port_args(const uint64_t addr, const uint64_t value);
        int set_port_cmd(const uint64_t value);

        int set_interrupt_signal();
        int set_polling_signal();

        int run(const int fd) const;

        std::string name () const;
        std::string type_string() const;
        std::string debug_string() const;

    private:
        std::unordered_map<uint64_t, FpgaPort> m_port_args;
        FpgaPort m_port_cmd;
        FpgaPort m_port_signal;
        uint64_t m_signal_switch;

        struct FpgaShortCommond m_short_cmd;
        struct FpgaLongCommond  m_long_cmd;

        std::string m_name;

        double m_total_exec_count;
        double m_total_exec_time;
        double m_max_exec_time;
};

class DmaAgent {
};

class XDmaAgent {
};

class FpgaDevice {
    public:
        FpgaDevice();
        ~FpgaDevice();

        int run_sync(struct FpgaCommond &cmd);
        int run_async(struct FpgaCommond &cmd);
        int sync();

        int start();
        void stop();

    private:
        std::string m_name;
        std::string m_device_file;

        ThreadPool       m_worker;
        MemoryPool<Task> m_mem;

        bool m_is_working;
};

} // namespace sfday

#endif // SFDA_API_FPGA_H_

