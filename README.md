# Open-source C library for handling two-way communication over Raymarine's SeaTalk protocol.

This is not officially supported in any way, shape or form by anyone. *Use this at your own risk!*

This could not have been possible without Thomas Knauf's absolutely indispensible SeaTalk reference found here: http://www.thomasknauf.de/seatalk.htm. Thank you, Thomas, for all the work it must have taken to work out the protocol details! I am in awe.

This should be treated as a device driver. It can be linked into an application or it can be installed as a Linux kernel extension.

## Using it within an application

    #include "seatalk_state.h"

* to write a sensor value to the SeaTalk bus call set_x_sensor where &lt;x&gt; is the name of a sensor (compass)
* to get device status from the SeaTalk bus call get_x_status where &lt;x&gt; is the name of a device (autopilot)

All sensor and status values time out after 5 seconds (configurable in seatalk_status.h).

To use it this way you will need to provide a seatalk_hardware_layer.c file that implements the four functions prototyped in seatalk_hardware_layer.h.

If using this under Linux you might want to consider using the kernel extension method instead as it reduces SeaTalk communications to file operations. In any case, under Linux this will need to be run as root in order to get access to the GPIO pins and interrupt handler.

If using this in a microcontroller you should be able to write your own MCU-specific seatalk_hardware_layer.c file to generate an interrupt when the start bit is received and to start the receive and transmit timers as needed.

## Using the Linux kernel extension

This has been tested on a Raspberry Pi. There is nothing in the source code that is obviously RPi-specific so in theory it should compile for any Linux machine that provides you with GPIO pins.

The Linux kernel extension (once compiled) makes this very easy. You 
To compile the kernel extension you will first need to have the kernel headers for your specific Linux version. That is beyond the scope of this README. Once you have that:

    $ cd seatalk/linux
    $ sudo make

Install the kernel extension using:

    $ sudo insmod seatalk.ko

Should you need to uninstall it use:

    $ sudo rmmod seatalk.ko

The kernel extension will add files in three directories:

    /sys/seatalk/sensors
    /sys/seatalk/status
    /sys/seatalk/commands

* To write a sensor value to the SeaTalk bus write its value to the appropriate file in /sys/seatalk/sensors (heading). Note that this can only be done from the root account (ie you need to use "sudo -i" or your program needs to be running as a daemon).
* To read a status value that has been broadcast on the SeaTalk bus read it from the appropriate file in /sys/seatalk/status (autopilot).
