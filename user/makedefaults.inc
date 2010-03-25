##### JoyOS User Makefile ####

# COMPILER CONFIGURATION
CC = avr-gcc
MCU = atmega128
OBJCOPY = avr-objcopy
AVRDUDE = avrdude
INCLUDES = -I$(TOPDIR)inc
LIBS = -ljoyos $(EXTRALIBS:%=-l%)
LIBDIR = $(TOPDIR)lib
SCANFOP =
PRINTFOP = -Wl,-u,vfprintf -lprintf_flt -lm
CFLAGS = -Wall -std=gnu99 -g -Os -mmcu=$(MCU)
LDFLAGS = -L$(LIBDIR) $(PRINTFOP) $(SCANFOP) $(MEMLAYOUT)
AVRDUDEFLAGS_USER = -c stk500v1 -p $(MCU) -P $(PORT) -b 19200 -V -F

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
	@$(CC) $(CFLAGS) $(OBJ) $(LIBS) -o $@ $(LDFLAGS)

%.hex: %.elf
	@echo "Generating hex file $@"
	@$(OBJCOPY) -S -O ihex -R .eeprom $< $@

clean:
	rm -f $(OBJ)
	rm -f *.hex *.elf
#%
