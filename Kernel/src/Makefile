ifneq ($(KERNELRELEASE),)
# call from kernel build system

obj-m	:= module_test1.o
obj-m	+= module_test2.o
obj-m	+= module_test3.o
obj-m	+= module_test4.o

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif

install:
	cp -p *.ko /tmp/
export:
	cp -p *.c /home/ens/AJ74860/Documents
clean:
	rm -rf *.o *~ core .depend .*.cmd  *.mod.c .tmp_versions #*.ko

depend .depend dep:
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -M *.c > .depend


ifeq (.depend,$(wildcard .depend))
include .depend
endif
