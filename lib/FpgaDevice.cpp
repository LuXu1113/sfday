// ****************************************************************************
// Description:
//
//     [description]
//
// Author:
//
//     Lu Xu (luxu_0519@126.com)
//
// ****************************************************************************

#include <errno.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <sstream>
#include <unordered_map>
#include "api/FpgaDevice.h"

using std::string;
using std::unordered_map;

namespace sfday {

enum SIGNAL_TYPE {
    INVALID   = 0,
    INTERRUPT = 1,
    POLLING   = 2,
};

static inline uint64_t rdtsc() {
    unsigned int lo, hi;
    __asm__ __volatile__("rdtsc":"=a"(lo),"=b"(hi));
    return ((uint64_t)hi << 32) | lo;
}

/******************************** FpgaPort ************************************/
FpgaPort::FpgaPort()
    : m_name(),
      m_addr(0),
      m_value(0) {}

FpgaPort::FpgaPort(const uint64_t addr)
    : m_name(),
      m_addr(addr),
      m_value(0) {}

FpgaPort::FpgaPort(const uint64_t addr, const string& name)
    : m_name(name),
      m_addr(addr),
      m_value(0) {}

FpgaPort::~FpgaPort() {}

int FpgaPort::set_name(const string& name) {
    m_name = name;
    return SUCCESS;
}

int FpgaPort::set_addr(const uint64_t addr) {
    m_addr = addr;
    return SUCCESS;
}

int FpgaPort::set_value(const uint64_t value) {
    m_value = value;
    return SUCCESS;
}

uint64_t FpgaPort::get_value() const {
    return m_value;
}

string FpgaPort::name() const {
    return m_name;
}

string FpgaPort::type_string() const {
    return "FpgaPort"; 
}

string FpgaPort::debug_string() const {
    std::ostringstream oss;
    oss << "{ name: " << m_name 
        << ", addr: " << m_addr
        << ", value: " << m_value << " }";
    return oss.str();
}

/****************************** FpgaFunction **********************************/

FpgaFunction::FpgaFunction()
    : m_port_args(),
      m_port_cmd(),
      m_port_signal(),
      m_signal_switch(INVALID),
      m_short_commond(),
      m_long_commond(),
      m_name(),
      m_total_exec_count(0.0),
      m_total_exec_time(0.0),
      m_max_exec_time(0.0) {}

FpgaFunction::FpgaFunction(const string& name)
    : m_port_args(),
      m_port_cmd(),
      m_port_signal(),
      m_signal_switch(INVALID),
      m_short_commond(),
      m_long_commond(),
      m_name(name),
      m_total_exec_count(0.0),
      m_total_exec_time(0.0),
      m_max_exec_time(0.0) {}

int FpgaFunction::add_port_args(const uint64_t addr) {
    int ret = SUCCESS;

    auto iter = m_port_args.find(addr); 
    if (m_port_args.end() == iter) {
        FpgaPort tmp(addr);
        m_port_args[addr] = tmp;
    } else {
        ret = ERR_DUPLICATE_KEY;
    }

    return 0;
}

int FpgaFunction::add_port_args(const uint64_t addr, const string& name) {
    int ret = SUCCESS;

    auto iter = m_port_args.find(addr); 
    if (m_port_args.end() == iter) {
        FpgaPort tmp(addr, name);
        m_port_args[addr] = tmp;
    } else {
        ret = ERR_DUPLICATE_KEY;
    }

    return ret;
}

int FpgaFunction::add_port_cmd(const uint64_t addr) {
    return m_port_cmd.set_addr(addr);
}

int FpgaFunction::add_port_signal(const uint64_t addr) {
    return m_port_signal.set_addr(addr);
}

int FpgaFunction::set_port_args(const uint64_t addr, const uint64_t value) {
    int ret = SUCCESS;

    auto iter = m_port_args.find(addr); 
    if (m_port_args.end() != iter) {
        ret = iter->second.set_value(value);
    } else {
        ret = ERR_INVALID_KEY;
    }

    return ret;
}

int FpgaFunction::set_port_cmd(const uint64_t value) {
    return m_port_cmd.set_value(value);
}

int FpgaFunction::set_interrupt_signal() {
    m_signal_switch = INTERRUPT;
    return SUCCESS;
}

int FpgaFunction::set_polling_signal() {
    m_signal_switch = POLLING;
    return SUCCESS;
}

string FpgaFunction::name() const {
    return m_name;
}

string FpgaFunction::type_string() const {
    return "FpgaFunction"; 
}

string FpgaFunction::debug_string() const {
    std::ostringstream oss;
    oss << "{ name: " << m_name 
        << ", total_exec_count: " << m_total_exec_count
        << ", total_exec_time: " << m_total_exec_time
        << ", max_exec_time: " << m_max_exec_time
        << ", ports: { ";

    for (auto iter = m_port_args.begin(); iter != m_port_args.end(); ++iter) {
        if (iter != m_port_args.begin()) {
            oss << ", ";
        }
        oss << iter->second.debug_string();
    }

    oss << "} }";

    return oss.str();
}

int FpgaFunction::run(const int handle) const {
    int ret = SUCCESS;
    
    if (INVALID == m_signal_switch) {
        ret = ERR_INVALID_SIGNAL_TYPE;
    } else if (m_port_args.size() <= MAXIMUM_ARGC_FOR_SHORT_COMMOND) {
        ret = ioctl(handle, FPGA_IOC_SHORT_CMD, &m_short_cmd);
    } else if (m_port_args.size() <= MAXIMUM_ARGC_FOR_LONG_COMMOND) {
        ret = ioctl(handle, FPGA_IOC_LONG_CMD, &m_long_cmd);
    } else {
        ret = E2BIG;
    }

    return ret;
}

/****************************** FpgaDevice ************************************/

FpgaDevice::FpgaDevice()
    : m_name(),
      m_record_exec_time(false),
      m_worker() {}

void FpgaDevice::set_record_exec_time() {
    m_record_exec_time = true;
}

void FpgaDevice::unset_record_exec_time() {
    m_record_exec_time = false;
}

int FpgaDevice::run_sync(struct FpgaCommond &cmd) {
    int ret = SUCCESS;

    if (cmd.m_record_exec_time) {
        struct timespec time1 = {0, 0};
        struct timespec time2 = {0, 0};

        clock_gettime(CLOCK_MONOTONIC, &ts1);
        ret = ioctl(m_device_file, FPGA_IOC_CMD, cmd);
        clock_gettime(CLOCK_MONOTONIC, &ts2);
        cmd.total_exec_ns = (ts2.tv_sec + ts2.tv_nsec) - (ts1.tv_sec + ts1.tv_nsec);
    } else {
        ret = ioctl(handle, FPGA_IOC_CMD, cmd);
    }

    return ret;
}

int FpgaDevice::run_async(struct FpgaCommond &cmd) {
    int ret = SUCCESS;

    do {

    } while (0);
    return ret;
}

}; // namespace sfday

