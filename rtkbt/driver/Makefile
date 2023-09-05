#KBUILD_CFLAGS += -D__linux__
EXTRA_CFLAGS=-fno-pic

ARCH ?=arm64
#CROSS_COMPILE_DIR := /home/z00356489/code/bt/device/hisilicon/bigfish/bluetooth/mt7662tu/driver
CROSS_COMPILE_DIR := $(shell pwd)

USB_MOD_NAME = btusb
USB_CFILES := \
	rtk_btusb.c 
$(USB_MOD_NAME)-objs := $(USB_CFILES:.c=.o)
obj-m := $(USB_MOD_NAME).o

 

#KDIR ?= $(shell pwd)/../../../../../output/Lenovo_android/mt5890_cn_android/rel/obj/kernel/chiling/kernel/linux-3.10/mt5890_android_smp_mod_defconfig
#KDIR ?= /home/z00356489/code/bt/out/target/product/Hi3751V551/obj/KERNEL_OBJ
#KDIR ?= $(shell pwd)/../../../../../../out/target/product/***/obj/KERNEL_OBJ
KDIR := $(ANDROID_PRODUCT_OUT)/obj/KERNEL_OBJ
KVER ?= 3.1.18

$(warning #########CHIPNAME#####$(CHIPNAME))

PWD := $(shell pwd)
CURFOLDER ?= $(pwd)

EXTRA_CFLAGS += -I$(CURFOLDER)/include -D__linux__

bt_usb:
	$(MAKE) -C $(KDIR) M=`pwd` ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) EXTRA_CLFAGS="-I$(CURFOLDER)/include -D__linux__ modules"

#bt_usb:
#	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	$(RM) Module.markers
	$(RM) modules.order

