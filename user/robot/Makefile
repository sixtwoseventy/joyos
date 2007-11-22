##### JoyOS User Makefile ####

# USER CONFIGURATION

# Source files - list all your .c files here (separated by spaces)
SRC = umain.c

# name of the generated file (no extension)
TARGET = robot

# USB-Serial port number for programming the happyboard
PORT = com4

# Top level JoyOS directory
TOPDIR = ../../

# COMPILER CONFIGURATION
CC = avr-gcc
MCU = atmega128
OBJCOPY = avr-objcopy
AVRDUDE = avrdude
INCLUDES = -I$(TOPDIR)inc
LIBS = $(TOPDIR)lib/*.o
SCANFOP =
CFLAGS = -Wall -std=gnu99 -g -Os -mmcu=$(MCU)
LDFLAGS = $(PRINTFOP) $(SCANFOP) $(MEMLAYOUT)
AVRDUDEFLAGS_USER = -c stk500 -p $(MCU) -P $(PORT) -b 19200

# Object files
OBJ = $(SRC:.c=.o)

all: $(TARGET).hex

program: $(TARGET).hex
	$(AVRDUDE) $(AVRDUDEFLAGS_USER) -U flash:w:$(TARGET).hex

%.o: %.c
	@echo "Compiling $@"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TARGET).elf: $(OBJ)
	@echo "Linking $@"
	@$(CC) $(CFLAGS) $(LIBS) $(OBJ) -o $@ $(LDFLAGS)

%.hex: %.elf
	@echo "Generating hex file $@"
	@$(OBJCOPY) -S -O ihex -R .eeprom $< $@

clean:
	rm -f $(OBJ)
	rm -f *.hex *.elf
#%
