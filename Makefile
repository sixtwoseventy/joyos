CC = avr-gcc
MCU = atmega128
OBJCOPY = avr-objcopy
AVRDUDE = avrdude
INCLUDES = -Isrc/inc
PRINTFOP = -Wl,-u,vfprintf -lprintf_min
#MEMLAYOUT = -Wl,--defsym=__malloc_heap_end=0x800800
CFLAGS = -Wall -std=gnu99 -g -Os -mmcu=$(MCU)
LDFLAGS = $(PRINTFOP) $(MEMLAYOUT)

AVRDUDE_PORT = /dev/tty.usbserial-0000113D
AVRDUDE_USERPORT = /dev/tty.usbserial-A20e1uZB
AVRDUDEFLAGS_BOOT = -c jtag1 -p $(MCU) -P $(AVRDUDE_PORT) -F
AVRDUDEFLAGS_USER = -c stk500 -p $(MCU) -P $(AVRDUDE_USERPORT) -F -b 19200

DEBUG_HOST = localhost:4242

OSNAME = bin/libjoyos
OSTARGET = $(OSNAME).hex
OSLIB = $(OSNAME).a
OSELF = $(OSNAME).elf

BOOTNAME = bin/hboot
BOOTTARGET = $(BOOTNAME).hex
BOOTELF = $(BOOTNAME).elf

# HAL source files
HALSRC = 	src/hal/io.c \
			src/hal/adc.c \
			src/hal/spi.c \
			src/hal/uart.c \
			src/hal/delay.c \

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
			src/drivers/rf.c \
			src/drivers/gyro.c \

# Kernel source files
KERNELSRC = src/kern/main.c \
			src/kern/lock.c \
			src/kern/board.c \
			src/kern/abort.c \
			src/kern/fork.c \
			src/kern/panic.c \
			src/kern/thread.c \
			src/kern/isr.c \
			src/kern/util.c \

# Library source files
LIBSRC = 	src/lib/async_printf.c \
		 	src/lib/pid.c

# User source files
USERSRC = user/umain.c

# Bootloader Source Files
BOOTSRC = 	src/boot/hboot.c \
			src/drivers/devices/at45db011.c \
			src/drivers/lcd.c \
			src/hal/uart.c \
			src/hal/spi.c \
			src/hal/adc.c \
			src/hal/io.c \

# ALL source files
SRC = $(DRIVERSRC) $(KERNELSRC) $(LIBSRC) $(HALSRC) $(USERSRC)

#Source files for library
DISTSRC = $(DRIVERSRC) $(KERNELSRC) $(LIBSRC) $(HALSRC)

# OS object files
OBJ = $(SRC:.c=.o)

# Bootloader object files
BOOTOBJ = $(BOOTSRC:.c=.o)

# Objects for library
DISTOBJ = $(DISTSRC:.c=.o)

all: $(OSLIB) $(BOOTTARGET) size

size: $(OSELF)
	@echo -n "-- OS Size "
	@tools/avr-mem.sh $(OSELF) $(MCU)

programnojoy: nojoy.hex
	$(AVRDUDE) $(AVRDUDEFLAGS_USER) -U flash:w:nojoy.hex

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

programterm:
	$(AVRDUDE) $(AVRDUDEFLAGS_BOOT) -t

%.o: %.c
	@echo "-- Compiling $@"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OSELF): $(OBJ)
	@echo "-- Linking $@"
	@$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)

$(BOOTELF): $(BOOTOBJ)
	@echo "-- Linking $@"
	@$(CC) $(CFLAGS) $(BOOTOBJ) -o $@ $(LDFLAGS) -Wl,--section-start=.text=0x1E000

%.hex: %.elf
	@echo "-- Generating hex file $@"
	@$(OBJCOPY) -S -O ihex -R .eeprom $< $@

$(OSLIB): $(DISTOBJ)
	@echo "-- Archiving" $@
	@avr-ar rcs $@ $(DISTOBJ)

clean:
	@echo "-- Cleaing objects"
	@rm -f $(OBJ) $(BOOTOBJ)
	@rm -f $(OSELF) $(OSTARGET) $(OSLIB)
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

debug: $(OSELF)gdb-config
	@echo "-- Starting debugger"
	@avarice -D -e -p --file $(OSELF) --jtag $(AVRDUDE_PORT) localhost:4242
	@avrgdb --command=gdbinit

docs:
	@echo "-- Generating documentation"
	@( cat doc/doxygen/Doxyfile ; ./tools/get_version.sh ) | doxygen -
	@cp doc/doxygen/reset.css doc/api
	@cp doc/doxygen/main.css doc/api
	@rm doc/api/tab* doc/api/doxygen.png

release: $(OSLIB) size docs
	@echo "-- Making release"
	@mkdir -p release
	@rm -rf release/*
	@mkdir release/6.270/
	@mkdir release/6.270/lib
	@mkdir release/6.270/src
	@cp -R src/inc release/6.270/inc
	@cp -R doc/api release/6.270/doc
	@cp bin/libjoyos.a release/6.270/lib
	@cp -R user/happytest release/6.270/src
	@cp -R user/gyrotest release/6.270/src
	@cp -R user/robot release/6.270/src
	@cp -R user/makedefaults.inc release/6.270/src
	@find release/ -name ".svn" | xargs rm -rf

dist: release
	@./tools/make_release.sh

distclean:
	@rm -rf dist/*
.PHONY: release
