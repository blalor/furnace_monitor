# Name: Makefile

CC = avr-gcc
CXX = avr-g++
OBJCOPY = @avr-objcopy

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
OBJECTS    = \
    src/8bit_tiny_timer0.o \
    src/usi_serial.o
    # src/8bit_tiny_timer1.o \
    # src/current_sensor.o \
    # src/relay.o \
    # src/scheduler.o \
    # src/zone_monitor.o \

# int. rc osc @ 8mhz [cksel=0010, sut=10]
# do not divide clock by 8
# bodlevel=101
FUSES = -U lfuse:w:0xE2:m -U hfuse:w:0xdd:m  -U efuse:w:0xff:m

TARGET_ARCH = -mmcu=$(DEVICE)

CPPFLAGS = -DF_CPU=$(CLOCK) -Isrc
CFLAGS   = --std=c99 -Wall -Werror -fdiagnostics-show-option -Os

# choose minimal printf implementation
# http://www.nongnu.org/avr-libc/user-manual/group__avr__stdio.html#gaa3b98c0d17b35642c0f3e4649092b9f1
# LDFLAGS += -Wl,-u,vfprintf -lprintf_min # … um, program size GROWS when I do that??

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)

.SUFFIXES: .elf .hex

# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.
.elf.hex:
	rm -f $@
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

# just messing around here; runs the C preprocessor
.c.S:
	$(COMPILE.c) -E $<

%.elf: %.o
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@

# symbolic targets:
all: main.hex

flash: all
	$(AVRDUDE) -U flash:w:main.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

# Xcode uses the Makefile targets "", "clean" and "install"
install: flash fuse

clean:
	rm -f main.hex main.elf main.o $(OBJECTS)

# file targets:
main.elf: main.o $(OBJECTS)

# Targets for code debugging and analysis:
disasm:	main.elf
	avr-objdump -d main.elf

# http://www.nongnu.org/avr-libc/user-manual/group__demo__project.html#demo_project_obj
dump: main.elf
	@avr-objdump -h -S main.elf

size: main.elf
	@avr-size --mcu=$(DEVICE) --format=avr main.elf
