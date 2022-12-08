obj-m := pstree.o

TARGET := $(obj-m:.o=)
MOD := $(obj-m:.o=).ko
pid ?= 1
LEN ?= 1000

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	make -C $(KDIR) M=$(PWD) modules

insmod: default
	sudo insmod $(MOD) pid=$(pid)
	sudo dmesg | tail -$(LEN)

# 如果通过 lsmod 查看模块，发现模块没有被卸载，可以使用 rmmod 命令卸载模块
clean:
	make -C $(KDIR) M=$(PWD) clean
	if [ $(shell lsmod | grep $(TARGET) | wc -l) -eq 1 ]; then \
		sudo rmmod $(TARGET); \
	fi

.PHONY: default insmod clean