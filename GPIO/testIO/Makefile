ARCH := arm
CROSS_COMPILE := arm-linux-
KDIR := /home/student/felabs/sysdev/tinysystem/linux-2.6.34/

obj-m += test.o

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) modules

clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) clean
