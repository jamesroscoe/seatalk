obj-m += seatalk_level_test.o
seatalk_level_test-y := seatalk_level_test_module.o ../seatalk_hardware_layer.o

KDIR = /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR)  M=$(shell pwd) modules

clean:
	make -C $(KDIR)  M=$(shell pwd) clean

modules_install: all
	#(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install
	$(DEPMOD)
