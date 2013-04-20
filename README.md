Bootjack
========

#What is Bootjack?#
Bootjack is a bootloader and configuration framework for AVR microprocessors.
It is designed to function much like the BIOS on your PC. It uses the avr109
protocol to communicate over serial (RS-232) with a host-based bootload client
(e.g. [AVRDUDE](http://www.nongnu.org/avrdude/)). Bootjack is written entirely
in C, which makes it very easy to read and modify. Despite the fact that its
written in C, Bootjack can fit into as little as 1KB of flash.

##Bootloader?##
The bootloader is a self-modifying program that allows the processor to flash
itself. Instead of using an ISP, JTAG, or HVPP programmer to flash the device,
the image can be transfered over a serial connection. This is especially
useful if you don't have a hardware programmer readily available.

##Configuration framework?##
Bootjack's configuration framework makes it easy to create command-line
interfaces. Menus are the core of the interface and can be easily defined with
the 'menu' structure. A simple example is provided for reference. When run, the
example menu looks like the following:

![Main Menu](/doc/assets/topmenu.png)  
![Submenu](/doc/assets/submenu.png)

#How do I build this?#
Note: Bootjack currently requires avr-gcc 4.7.1 or greater to build. Support
for avr-gcc 4.7.0 and earlier is coming soon.  

The following packages are needed to build:

- make
- avr-gcc >= 4.7.1
- avr-libc >= 1.8

Once these have been installed, run `make` from Bootjack's root directory.
This will build the default project and create `build/bootjack.elf` and
`build/bootjack.hex`. When changing target architectures or build options, its
easiest to copy and rename `config/default.mk` and then modify the first line
of the main Makefile (`CONFIG = default`) to point to the new config. For
example, copy `config/default.mk` to `config/myproject.mk` and change
`CONFIG = default` to `CONFIG = myproject`.
