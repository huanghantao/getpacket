#Filename: Makefile
CONFIG_MODULE_SIG=n
	
obj-m := getpacket_k.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
clean:
	rm *o