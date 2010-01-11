# User source files
USERSRC = tests/rftest.c
#AVRDUDE_PORT = /dev/tty.usbserial-0000113D
AVRDUDE_PORT ?= /dev/ttyUSB0
#AVRDUDE_USERPORT = /dev/tty.usbserial-A20e1uZB
AVRDUDE_USERPORT ?= /dev/ttyUSB0

CC = avr-gcc
MCU = atmega128
OBJCOPY = avr-objcopy
AVRDUDE = avrdude
FTDI_EEPROM = ftdi_eeprom

INCLUDES = -Isrc/inc
# Two printf options: full for OS, minimal for bootloader
BOOT_PRINTFOP = -Wl,-u,vfprintf -lprintf_min
OS_PRINTFOP = -Wl,-u,vfprintf -lprintf_flt -lm
#MEMLAYOUT = -Wl,--defsym=__malloc_heap_end=0x800800
#OSMEMLAYOUT = -Wl,--defsym=__heap_start=0x808000,--defsym=__heap_end=0x80ffff
#^^Hopefully makes heap appear in external RAM.
CFLAGS = -Wall -std=gnu99 -g -Os -mmcu=$(MCU)
BOOT_LDFLAGS = $(BOOT_PRINTFOP) $(MEMLAYOUT)
OS_LDFLAGS = $(OS_PRINTFOP) $(OSMEMLAYOUT) 

AVRDUDEFLAGS_BOOT = -c jtag1 -p $(MCU) -P $(AVRDUDE_PORT) -F
AVRDUDEFLAGS_USER = -c stk500 -p $(MCU) -P $(AVRDUDE_USERPORT) -F -b 19200

DEBUG_HOST = localhost:4242

OSNAME = bin/libjoyos
OSTARGET = $(OSNAME).hex
OSLIB = $(OSNAME).a
OSELF = $(OSNAME).elf

HLNAME = bin/libhappy
HLLIB = $(HLNAME).a
HLELF = $(HLNAME).elf

BOOTNAME = bin/hboot
BOOTTARGET = $(BOOTNAME).hex
BOOTELF = $(BOOTNAME).elf

# HAL source files
HALSRC = 	src/hal/io.c \
			src/hal/adc.c \
			src/hal/spi.c \
			src/hal/uart.c \
			src/hal/delay.c \
			src/hal/i2c.c \

# Driver source files
DRIVERSRC = src/drivers/devices/fpga.c \
			src/drivers/devices/mcp3008.c \
			src/drivers/devices/nrf24l01.c \
			src/drivers/devices/at45db011.c \
			src/drivers/lcd.c \
			src/drivers/servo.c \
			src/drivers/motor.c \
			src/drivers/analog.c \
			src/drivers/digital.c \
			src/drivers/encoder.c \
			src/drivers/buttons.c \
			src/drivers/gyro.c \
			src/drivers/rf.c \

# Kernel source files
KERNELSRC = src/kern/main.c \
			src/kern/lock.c \
			src/kern/board.c \
			src/kern/fork.c \
			src/kern/panic.c \
			src/kern/thread.c \
			src/kern/isr.c \
			src/kern/util.c \

# Library source files
LIBSRC = 	src/lib/async_printf.c \
		 	src/lib/pid.c \
			src/lib/irdist.c \
			src/lib/confdb.c \
			src/lib/happylib.c \
			src/lib/motor_group.c \
			src/lib/motion.c \

# Bootloader Source Files
BOOTSRC = 	src/boot/hboot.c \
			src/drivers/devices/at45db011.c \
			src/drivers/lcd.c \
			src/hal/uart.c \
			src/hal/spi.c \
			src/hal/adc.c \
			src/hal/io.c \
			src/hal/delay.c \

# ALL source files
SRC = $(DRIVERSRC) $(KERNELSRC) $(HALSRC) $(USERSRC)

# Source files for os library
DISTSRC = $(DRIVERSRC) $(KERNELSRC) $(HALSRC)

# Source files for happy library
HLSRC = $(LIBSRC)

# OS object files
OBJ = $(SRC:.c=.o)

# Happylib object files
HLOBJ = $(HLSRC:.c=.o)

# Bootloader object files
BOOTOBJ = $(BOOTSRC:.c=.o)

# Objects for library
DISTOBJ = $(DISTSRC:.c=.o)

all: $(OSLIB) $(HLLIB) $(BOOTTARGET) size

size: $(OSELF)
	@echo -n "-- OS Size "
	@tools/avr-mem.sh $(OSELF) $(MCU)

program: $(OSTARGET)
	$(AVRDUDE) $(AVRDUDEFLAGS_USER) -U flash:w:$(OSTARGET)

programboot: $(BOOTTARGET)
	$(AVRDUDE) $(AVRDUDEFLAGS_BOOT) -U flash:w:$(BOOTTARGET)

#        76543210
# hfuse: 10011000 0x98
# lfuse: 11101111 0xef
# efuse: 11111111 0xff

programfuses:
	$(AVRDUDE) $(AVRDUDEFLAGS_BOOT) -V -U lfuse:w:0xef:m -U hfuse:w:0x98:m -U efuse:w:0xff:m

programftdi:
	$(FTDI_EEPROM) --flash-eeprom tools/ftdi.conf

programterm:
	$(AVRDUDE) $(AVRDUDEFLAGS_BOOT) -t

%.o: %.c
	@echo "-- Compiling $@"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OSELF): $(OBJ) $(HLOBJ)
	@echo "-- Linking $@"
	@$(CC) $(CFLAGS) $(OBJ) $(HLOBJ) -o $@ $(OS_LDFLAGS)

$(BOOTELF): $(BOOTOBJ)
	@echo "-- Linking $@"
	@$(CC) $(CFLAGS) $(BOOTOBJ) -o $@ $(BOOT_LDFLAGS) -Wl,--section-start=.text=0x1E000

%.hex: %.elf
	@echo "-- Generating hex file $@"
	@$(OBJCOPY) -S -O ihex -R .eeprom $< $@

$(OSLIB): $(DISTOBJ)
	@echo "-- Archiving" $@
	@avr-ar rcs $@ $(DISTOBJ)

$(HLLIB): $(HLOBJ)
	@echo "-- Archiving" $@
	@avr-ar rcs $@ $(HLOBJ)

clean:
	@echo "-- Cleaing objects"
	@rm -f $(OBJ) $(BOOTOBJ) $(HLOBJ)
	@rm -f $(OSELF) $(OSTARGET) $(OSLIB) $(HLLIB)
	@rm -f $(BOOTELF) $(BOOTTARGET)
	@rm -f gdbinit
	@rm -rf release/*
	@rm -rf doc/api/*

gdb-config:
	@rm -f gdbinit
	@echo "define reset" >> gdbinit
	@echo "SIGNAL SIGHUP" >> gdbinit
	@echo "end" >> gdbinit
	@echo "file $(OSELF)" >> gdbinit
	@echo "target remote localhost:4242" >> gdbinit
	@echo "break main" >> gdbinit

debug: $(OSELF) gdb-config
	@echo "-- Starting debugger"
	@avarice -D -e -p --file $(OSELF) --jtag $(AVRDUDE_PORT) localhost:4242
	@avr-gdb --command=gdbinit

docs:
	@echo "-- Generating documentation"
	@( cat doc/doxygen/Doxyfile ; ./tools/get_version.sh ) | doxygen -
	@cp doc/doxygen/reset.css doc/api
	@cp doc/doxygen/main.css doc/api
	@rm doc/api/tab* doc/api/doxygen.png

release: $(OSLIB) $(HLLIB) size docs
	@echo "-- Making release"
	@mkdir -p release
	@rm -rf release/*
	@mkdir release/6.270/
	@mkdir release/6.270/lib
	@mkdir release/6.270/src
	@cp -R src/inc release/6.270/inc
	@cp -R doc/api release/6.270/doc
	@cp bin/*.a release/6.270/lib
	@cp -R user/irdistcal release/6.270/src
	@cp -R user/irdisttest release/6.270/src
	@cp -R user/happytest release/6.270/src
	@cp -R user/gyrotest release/6.270/src
	@cp -R user/exercises release/6.270/src
	#@cp -R user/motiontest release/6.270/src
	@cp -R user/robot release/6.270/src
	@cp -R user/makedefaults.inc release/6.270/src
	@find release/ -name ".svn" | xargs rm -rf

dist: release
	@./tools/make_release.sh

distclean:
	@rm -rf dist/*
.PHONY: release
