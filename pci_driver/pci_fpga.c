#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include "common/errno.h"

static int __init pci_driver_init(void) {
    int ret = SUCCESS;

    do {
        ret = register_pci_device();
        if (SUCCESS != ret) {
            break;
        }

        ret = register_blk_device();
        if (SUCCESS != ret) {
            break;
        }

    } while (0);

    return ret;
}

static void __exit pci_driver_exit(void) {
}

module_init(pci_driver_init);
module_exit(pci_driver_exit);

MODULE_AUTHOR("Lu Xu");
MODULE_LICENSE("GPL");

