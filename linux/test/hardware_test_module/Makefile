obj-m += seatalk_test.o
seatalk_test-y := ../../../test/hardware/seatalk_hardware_test.o seatalk_hardware_test_module.o ../../../seatalk_transport_layer.o

KDIR = /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR)  M=$(shell pwd) modules

clean:
	make -C $(KDIR)  M=$(shell pwd) clean

modules_install: all
	#(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install
	$(DEPMOD)
