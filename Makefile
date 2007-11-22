CC = avr-gcc
MCU = atmega128
OBJCOPY = avr-objcopy
AVRDUDE = avrdude
INCLUDES = -Idrivers -Icommon -Iinc
#PRINTFOP = -Wl,-u,vfprintf -lprintf_min
#SCANFOP = -Wl,-u,vfscanf -lscanf_flt -lm
SCANFOP =
#MEMLAYOUT = -Wl,--defsym=__malloc_heap_end=0x800800
CFLAGS = -Wall -std=gnu99 -g -Os -mmcu=$(MCU)
LDFLAGS = $(PRINTFOP) $(SCANFOP) $(MEMLAYOUT)
#AVRDUDEFLAGS = -c jtag1 -p $(MCU) -P com4
AVRDUDEFLAGS = -c jtag1 -p $(MCU) -P /dev/ttyUSB0 -F
#AVRDUDEFLAGS_USER = -c stk500 -p $(MCU) -P com7 -b 19200
AVRDUDEFLAGS_USER = -c stk500 -p $(MCU) -P /dev/ttyUSB0 -b 19200 -F

OSTARGET = os.hex

# OS Source files

# Driver source files
DRIVERSRC = common/io.c \
			drivers/lcd.c \
			drivers/adc.c \
			drivers/spi.c \
			drivers/uart.c \
			drivers/fpga.c \
			drivers/servo.c \
			drivers/motor.c \
			drivers/analog.c \
			drivers/digital.c \
			drivers/encoder.c \
			drivers/mcp3008.c \
			drivers/max1230.c \
			drivers/nrf24l01.c \
			drivers/at45db011.c \
			drivers/rf.c \
			drivers/gyro.c

# Kernel source files
KERNELSRC = kern/main.c \
			kern/lock.c \
			kern/board.c \
			kern/abort.c \
			kern/fork.c \
			kern/panic.c \
			kern/thread.c \
			kern/isr.c \
			kern/util.c \

# Library source files
LIBSRC = lib/async_printf.c \
			lib/pid.c

# User source files
USERSRC = user/umain.c

SRC = $(DRIVERSRC) $(KERNELSRC) $(LIBSRC) $(USERSRC)
DISTSRC = $(DRIVERSRC) $(KERNELSRC) $(LIBSRC)

# OS Object files
OBJ = $(SRC:.c=.o)

DISTOBJ = $(DISTSRC:.c=.o)

all: $(OSTARGET)

dist: os.a

program: $(OSTARGET)
	$(AVRDUDE) $(AVRDUDEFLAGS_USER) -U flash:w:$(OSTARGET)

programfuses:
	$(AVRDUDE) $(AVRDUDEFLAGS) -U lfuse:w:0xef:m -U hfuse:w:0x98:m -U efuse:w:0xff:m

programterm:
	$(AVRDUDE) $(AVRDUDEFLAGS) -t

%.o: %.c
	@echo "Compiling $@"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

os.elf: $(OBJ)
	@echo "Linking $@"
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)

%.hex: %.elf
	@echo "Generating hex file $@"
	$(OBJCOPY) -S -O ihex -R .eeprom $< $@

os.a: $(DISTOBJ)
	@echo
	@echo "Creating lib" %@
	@avr-ar rvs $@ $(DISTOBJ)

clean:
	rm -f $(OBJ)
	rm -f *.hex *.elf
	rm -rf release/*
	rm -rf doc/api/*

docs:
	#c:/Program\ Files/doxygen/bin/doxygen.exe
	doxygen

release: $(OSTARGET) docs
	rm -rf release/*
	mkdir release/6.270/
	cp -R inc release/6.270/inc
	cp -R common/*.h release/6.270/inc
	rm -rf release/6.270/inc/.svn
	cp -R doc release/6.270/doc
	rm -rf release/6.270/doc/.svn
	rm -rf release/6.270/doc/api/.svn
	mkdir release/6.270/lib
	cp -R drivers/*.o release/6.270/lib/
	cp -R kern/*.o release/6.270/lib/
	cp -R lib/*.o release/6.270/lib/
	cp -R common/*.o release/6.270/lib/
	mkdir release/6.270/src
	cp -R user/happytest release/6.270/src
	rm -rf release/6.270/src/happytest/.svn
	cp -R user/gyrotest release/6.270/src
	rm -rf release/6.270/src/gyrotest/.svn
	cp -R user/robot release/6.270/src
	rm -rf release/6.270/src/robot/.svn
	cp 6270.ppg release/6.270/

