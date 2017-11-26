#ifndef SFDAY_COMMON_COMM_H_
#define SFDAY_COMMON_COMM_H_

#define PCIE_XILINX_VENDOR_ID  0x10ee

#define PCIE_XCKC705_DEVICE_ID 0x7021
#define PCIE_XCK480T_DEVICE_ID 0x7028
#define PCIE_XCKU115_DEVICE_ID 0x8038



#define MAXIMUM_ARGS  64

struct FpgaCommond {
    uint64_t num_args;
    uint64_t port[MAXIMUM_ARGC_OF_SHORT_COMMOND];
    uint64_t value[MAXIMUM_ARGC_OF_SHORT_COMMOND];
    uint64_t cmd_port;
    uint64_t cmd_value;
    uint64_t polling_port;
    uint64_t polling_mask;
    uint64_t interrupt_port;
    uint64_t interrupt_mask;
    uint64_t device_exec_ns;
    uint64_t kenel_exec_ns;
    uint64_t total_exec_ns;
};

#define FPGA_IOC_MAGIC 0x13
#define FPGA_IOC_BASE  0

// #define FPGA_IOC_REG_READ         _IOW(FPGA_IOC_MAGIC, FPGA_IOC_BASE + 1,  struct FpgaRegRead *)
// #define FPGA_IOC_REG_WRITE        _IOW(FPGA_IOC_MAGIC, FPGA_IOC_BASE + 2,  struct FpgaRegWrite *)
// #define FPGA_IOC_KMEM_ALLOC       _IOW(FPGA_IOC_MAGIC, FPGA_IOC_BASE + 3,  struct FpgaKmemAlloc *)
// #define FPGA_IOC_KMEM_FREE        _IOW(FPGA_IOC_MAGIC, FPGA_IOC_BASE + 4,  struct FpgaKmemFree *)
// #define FPGA_IOC_DEVICE_MEM_ALLOC _IOW(FPGA_IOC_MAGIC, FPGA_IOC_BASE + 5,  struct FpgaKmemAlloc *)
// #define FPGA_IOC_DEVICE_MEM_FREE  _IOW(FPGA_IOC_MAGIC, FPGA_IOC_BASE + 6,  struct FpgaKmemFree *)
// #define FPGA_IOC_DMA              _IOW(FPGA_IOC_MAGIC, FPGA_IOC_BASE + 7,  struct FpgaDmaCommond *)
// #define FPGA_IOC_XDMA             _IOW(FPGA_IOC_MAGIC, FPGA_IOC_BASE + 8,  struct FpgaXDmaCommond *)
// #define FPGA_IOC_SHORT_CMD        _IOW(FPGA_IOC_MAGIC, FPGA_IOC_BASE + 9,  struct FpgaShortCommond *)
#define FPGA_IOC_CMD              _IOW(FPGA_IOC_MAGIC, FPGA_IOC_BASE + 10, struct FpgaCommond *)

#endif

