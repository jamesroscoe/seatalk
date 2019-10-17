obj-m += seatalk.o
seatalk-y := seatalk_module.o seatalk_hardware_layer.o ../seatalk_state.o ../seatalk_transport_layer.o ../seatalk_datagram.o ../seatalk_command.o

KDIR = /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR)  M=$(shell pwd) modules

clean:
	make -C $(KDIR)  M=$(shell pwd) clean

modules_install: all
	#(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install
	$(DEPMOD)
