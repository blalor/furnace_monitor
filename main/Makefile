# Name: Makefile

CC = avr-gcc
CXX = avr-g++
AR = avr-ar

OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size

AVRDUDE = avrdude 


# DEVICE ....... The AVR device you compile for
# CLOCK ........ Target AVR clock rate in Hertz
# OBJECTS ...... The object files created from your source files. This list is
#                usually the same as the list of source files with suffix ".o".
# PROGRAMMER ... Options to avrdude which define the hardware you use for
#                uploading to the AVR and the interface where this hardware
#                is connected.
# FUSES ........ Parameters for avrdude to flash the fuses appropriately.

DEVICE     = attiny85
CLOCK      = 8000000
PROGRAMMER = -c avrispmkii -P usb

# int. rc osc @ 8mhz [cksel=0010, sut=10]
# do not divide clock by 8
# bodlevel=101
FUSES = -U lfuse:w:0xE2:m -U hfuse:w:0xdd:m -U efuse:w:0xff:m

TARGET_ARCH = -mmcu=$(DEVICE)

CPPFLAGS = -DF_CPU=$(CLOCK) -Isrc
CFLAGS   = --std=c99 -Wall -Werror -fdiagnostics-show-option -Os
ARFLAGS  = rvs

# choose minimal printf implementation
# http://www.nongnu.org/avr-libc/user-manual/group__avr__stdio.html#gaa3b98c0d17b35642c0f3e4649092b9f1
# LDFLAGS += -Wl,-u,vfprintf -lprintf_min # … um, program size GROWS when I do that??

.SUFFIXES: .elf .hex _size _dis _dump _flash

# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.
.elf.hex:
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

# just messing around here; runs the C preprocessor
.c.S:
	$(COMPILE.c) -E $<

%.elf: %.o
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@

%.a:
	$(AR) $(ARFLAGS) $@ $?

# Targets for code debugging and analysis

# show size
.elf_size:
	$(SIZE) --mcu=$(DEVICE) --format=avr $<

# disassemble
.elf_dis:
	$(OBJDUMP) -d $<

# http://www.nongnu.org/avr-libc/user-manual/group__demo__project.html#demo_project_obj
.elf_dump:
	$(OBJDUMP) -h -S $<

.hex_flash:
	$(AVRDUDE) $(PROGRAMMER) -p $(DEVICE) -U flash:w:$<:i

# symbolic targets:
all: main.hex

fuse:
	$(AVRDUDE) $(PROGRAMMER) -p $(DEVICE) $(FUSES)

# Xcode uses the Makefile targets "", "clean" and "install"
#install: fuse main_flash 

clean:
	rm -f main.hex main.elf *.o lib*.a src/*.o 

# file targets:
libtimer.a: src/8bit_tiny_timer0.o src/8bit_tiny_timer1.o
libusiserial.a: src/usi_serial.o
libscheduler.a: src/scheduler.o
libdevices.a: src/furnace_power_sensor.o src/relay.o src/zone_monitor.o
libserialprotocol.a: src/serial_handler.o
libfurnacecontroller.a: src/controller.o

main.elf: libtimer.a libusiserial.a libscheduler.a libdevices.a \
          libserialprotocol.a libfurnacecontroller.a

