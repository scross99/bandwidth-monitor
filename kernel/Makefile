ifeq ($(KERNELRELEASE),)

    KERNELDIR ?= /lib/modules/$(shell uname -r)/build
    PWD := $(shell pwd)

.PHONY: module clean

module:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -rf *.o core .depend .*.cmd *.ko *.mod.c .tmp_versions Module.symvers modules.order

else
    $(info Building with KERNELRELEASE = ${KERNELRELEASE})

    # called from kernel build system: just declare what our modules are

    obj-m := bandwidth.o

endif

