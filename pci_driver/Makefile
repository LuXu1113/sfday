obj-m += pci_fpga.o

all: pci_fpga

KERNEL_DIR := /usr/src/kernels/3.10.0-693.el7.x86_64

pci_fpga: pci_fpga.c
	make -C $(KERNEL_DIR) M=$(PWD) modules

clean:
	rm -f *.a *.o *.ko modules.order Module.symvers

