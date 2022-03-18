obj-m := ch2_mod2_201912698.o

KBUILD_EXTRA_SyMBOLS := /home/lackms/바탕화면/limsiso/Symbol/mod1/Module.symvers
KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
