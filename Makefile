# User source files
#USERSRC = user/happytest/happytest.c
#USERSRC = user/playingField2012/main.c
USERSRC = user/playingField2012/rfmon.c

# Serial port
AVRDUDE_USERPORT ?= `ls /dev/tty.usb*`

# Mac Users - you may need to change this to stk500 if you get an error when programming
PROGRAMMER = stk500









####################################################
#   NOTHING NEEDS TO BE CHANGED BELOW THIS POINT   #
####################################################

# Serial port for JTAG programmer
AVRDUDE_PORT ?= /dev/tty.usbserial-A100099c


# Use Arduino Toolchain if on Darwin
ifeq (${shell uname}, Darwin)
AVR_ROOT := "/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/"
AVR_INC := ${AVR_ROOT}/avr-4/
AVR := ${AVR_ROOT}/bin/avr
LFLAGS += -L/opt/local/lib
AVRDUDE_CONFIG = -C "${AVR_ROOT}etc/avrdude.conf"
else
AVR_ROOT := /usr/lib/avr
AVR_INC := ${AVR_ROOT}
AVR := avr
AVRDUDE_CONFIG = "" #default to the systemwide config
endif


CC = ${AVR}-gcc
OBJCOPY = ${AVR}-objcopy
AVRDUDE = ${AVR}dude
AR = ${AVR}-ar
AVARICE = ${AVR}rice
AVRGDB = ${AVR}-gdb

MCU = atmega128
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

AVRDUDEFLAGS_BOOT = -c jtag1 -p $(MCU) -P $(AVRDUDE_PORT) -F $(AVRDUDE_CONFIG)
AVRDUDEFLAGS_USER = -c $(PROGRAMMER) -p $(MCU) -P $(AVRDUDE_USERPORT) -F -b 19200 -V $(AVRDUDE_CONFIG)

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
			src/kern/ring.c \

# Library source files
LIBSRC = 	src/lib/pid.c \
			src/lib/irdist.c \
			src/lib/confdb.c \
			src/lib/happylib.c \
			src/lib/motor_group.c \
			src/lib/motion.c \

# Bootloader Source Files
BOOTSRC = 	src/boot/hboot.c \
			src/drivers/devices/at45db011.c \
			src/drivers/lcd_hboot.c \
			src/hal/uart_hboot.c \
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

all: $(OSLIB) $(HLLIB) $(BOOTTARGET) size docs

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
	@mkdir -p bin
	@$(CC) $(CFLAGS) $(OBJ) $(HLOBJ) -o $@ $(OS_LDFLAGS)

$(BOOTELF): $(BOOTOBJ)
	@echo "-- Linking $@"
	@mkdir -p bin
	@$(CC) $(CFLAGS) $(BOOTOBJ) -o $@ $(BOOT_LDFLAGS) -Wl,--section-start=.text=0x1E000

%.hex: %.elf
	@echo "-- Generating hex file $@"
	@$(OBJCOPY) -S -O ihex -R .eeprom $< $@

$(OSLIB): $(DISTOBJ)
	@echo "-- Archiving" $@
	@$(AR) rcs $@ $(DISTOBJ)

$(HLLIB): $(HLOBJ)
	@echo "-- Archiving" $@
	@$(AR) rcs $@ $(HLOBJ)

clean:
	@echo "-- Cleaning objects"
	@rm -f $(OBJ) $(BOOTOBJ) $(HLOBJ)
	@rm -f $(OSELF) $(OSTARGET) $(OSLIB) $(HLLIB)
	@rm -f $(BOOTELF) $(BOOTTARGET)
	@rm -f gdbinit
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
	@$(AVARICE) -D -e -p --file $(OSELF) --jtag $(AVRDUDE_PORT) localhost:4242
	@$(AVRGDB) --command=gdbinit

docs:
	@echo "-- Generating documentation"
	@( cat doc/doxygen/Doxyfile ; ./tools/get_version.sh ) | doxygen -
	@cp doc/doxygen/reset.css doc/api
	@cp doc/doxygen/main.css doc/api
	@rm doc/api/tab* doc/api/doxygen.png

distclean:
	@rm -rf dist/*

simulate:
	@gcc $(SRC) src/drivers/socket.c $(INCLUDES) -o client -D SIMULATE -lpthread

.PHONY: all
