#! /bin/sh
# Utility to print out sizes of the different memory spaces
#   of an ELF file for the AVR target.
# Written by Jörg Wunsch and Eric B. Weddington
# Released to the Public Domain


AWK=gawk
AVRSIZE=avr-size

# Usage
if test $# -lt 1; then
    echo "Usage: avr-mem.sh <ELF file> [<AVR device name>]" >&2
    echo "Prints sizes of the different AVR memory spaces in an ELF file." >&2
    exit 1
fi

# Memory size variables
PROGMAX=0
DATAMAX=0
EEPROMMAX=0

# Fixed sizes.
AVR64=64
AVR128=128
AVR256=256
AVR512=512
AVR1K=1024
AVR2K=2048
AVR4K=4096
AVR8K=8192
AVR16K=16384
AVR24K=24576
AVR32K=32768
AVR64K=65536
AVR128K=131072


# Table of AVR memory sizes.
case "$2" in

"atmega128")  PROGMAX=${AVR128K}; DATAMAX=${AVR4K}; EEPROMMAX=${AVR4K};;
"atmega103")  PROGMAX=${AVR128K}; DATAMAX=4000; EEPROMMAX=${AVR4K};;

"atmega64")   PROGMAX=${AVR64K}; DATAMAX=${AVR4K}; EEPROMMAX=${AVR2K};;
"atmega645")  PROGMAX=${AVR64K}; DATAMAX=${AVR4K}; EEPROMMAX=${AVR2K};;
"atmega649")  PROGMAX=${AVR64K}; DATAMAX=${AVR4K}; EEPROMMAX=${AVR2K};;
"atmega6450") PROGMAX=${AVR64K}; DATAMAX=${AVR4K}; EEPROMMAX=${AVR2K};;
"atmega6490") PROGMAX=${AVR64K}; DATAMAX=${AVR4K}; EEPROMMAX=${AVR2K};;

"atmega32")   PROGMAX=${AVR32K}; DATAMAX=${AVR2K}; EEPROMMAX=${AVR1K};;
"atmega323")  PROGMAX=${AVR32K}; DATAMAX=${AVR2K}; EEPROMMAX=${AVR1K};;
"atmega325")  PROGMAX=${AVR32K}; DATAMAX=${AVR2K}; EEPROMMAX=${AVR1K};;
"atmega329")  PROGMAX=${AVR32K}; DATAMAX=${AVR2K}; EEPROMMAX=${AVR1K};;
"atmega3250") PROGMAX=${AVR32K}; DATAMAX=${AVR2K}; EEPROMMAX=${AVR1K};;
"atmega3290") PROGMAX=${AVR32K}; DATAMAX=${AVR2K}; EEPROMMAX=${AVR1K};;

"atmega16")   PROGMAX=${AVR16K}; DATAMAX=${AVR1K}; EEPROMMAX=${AVR512};;
"atmega161")  PROGMAX=${AVR16K}; DATAMAX=${AVR1K}; EEPROMMAX=${AVR512};;
"atmega162")  PROGMAX=${AVR16K}; DATAMAX=${AVR1K}; EEPROMMAX=${AVR512};;
"atmega163")  PROGMAX=${AVR16K}; DATAMAX=${AVR1K}; EEPROMMAX=${AVR512};;
"atmega165")  PROGMAX=${AVR16K}; DATAMAX=${AVR1K}; EEPROMMAX=${AVR512};;
"atmega168")  PROGMAX=${AVR16K}; DATAMAX=${AVR1K}; EEPROMMAX=${AVR512};;
"atmega169")  PROGMAX=${AVR16K}; DATAMAX=${AVR1K}; EEPROMMAX=${AVR512};;

"at90c8534")  PROGMAX=${AVR8K}; DATAMAX=352; EEPROMMAX=${AVR512};;
"at90s8515")  PROGMAX=${AVR8K}; DATAMAX=${AVR512}; EEPROMMAX=${AVR512};;
"at90s8535")  PROGMAX=${AVR8K}; DATAMAX=${AVR512}; EEPROMMAX=${AVR512};;
"atmega8")    PROGMAX=${AVR8K}; DATAMAX=${AVR1K}; EEPROMMAX=${AVR512};;
"atmega8515") PROGMAX=${AVR8K}; DATAMAX=${AVR512}; EEPROMMAX=${AVR512};;
"atmega8535") PROGMAX=${AVR8K}; DATAMAX=${AVR512}; EEPROMMAX=${AVR512};;
"atmega88")   PROGMAX=${AVR8K}; DATAMAX=${AVR1K}; EEPROMMAX=${AVR512};;

"at90s4414")  PROGMAX=${AVR4K}; DATAMAX=352; EEPROMMAX=${AVR256};;
"at90s4433")  PROGMAX=${AVR4K}; DATAMAX=${AVR128}; EEPROMMAX=${AVR256};;
"at90s4434")  PROGMAX=${AVR4K}; DATAMAX=352; EEPROMMAX=${AVR256};;
"atmega48")   PROGMAX=${AVR4K}; DATAMAX=${AVR512}; EEPROMMAX=${AVR256};;

"at90s2313")  PROGMAX=${AVR2K}; DATAMAX=${AVR128}; EEPROMMAX=${AVR128};;
"at90s2323")  PROGMAX=${AVR2K}; DATAMAX=${AVR128}; EEPROMMAX=${AVR128};;
"at90s2333")  PROGMAX=${AVR2K}; DATAMAX=224; EEPROMMAX=${AVR128};;
"at90s2343")  PROGMAX=${AVR2K}; DATAMAX=${AVR128}; EEPROMMAX=${AVR128};;
"attiny22")   PROGMAX=${AVR2K}; DATAMAX=224; EEPROMMAX=${AVR128};;
"attiny2313") PROGMAX=${AVR2K}; DATAMAX=${AVR128}; EEPROMMAX=${AVR128};;
"attiny26")   PROGMAX=${AVR2K}; DATAMAX=${AVR128}; EEPROMMAX=${AVR128};;
"attiny28")   PROGMAX=${AVR2K};;

"at90s1200")  PROGMAX=${AVR1K}; EEPROMMAX=${AVR64};;
"attiny11")   PROGMAX=${AVR1K}; EEPROMMAX=${AVR64};;
"attiny12")   PROGMAX=${AVR1K}; EEPROMMAX=${AVR64};;
"attiny13")   PROGMAX=${AVR1K}; DATAMAX=${AVR64}; EEPROMMAX=${AVR64};;
"attiny15")   PROGMAX=${AVR1K}; EEPROMMAX=${AVR64};;

"at94k")      PROGMAX=${AVR32K}; DATAMAX=${AVR4K};;
"at76c711")   PROGMAX=${AVR16K}; DATAMAX=${AVR2K};;
"at43usb320") PROGMAX=${AVR128K}; DATAMAX=608;;
"at43usb355") PROGMAX=${AVR24K}; DATAMAX=1120;;
"at86rf401")  PROGMAX=${AVR2K}; DATAMAX=224; EEPROMMAX=${AVR128};;
"at90can128") PROGMAX=${AVR128K}; DATAMAX=${AVR4K}; EEPROMMAX=${AVR4K};;

esac


${AVRSIZE} -A "$1" | ${AWK} -v progmax=${PROGMAX} -v \
datamax=${DATAMAX} -v eeprommax=${EEPROMMAX} -v device=$2 -- '
/^\.(text|data|bootloader) / {text += $2}
/^\.(data|bss|noinit) / {data += $2}
/^\.(eeprom) / {eeprom += $2}
END {
    if (device != "")
    {
        printf "%s usage: ", device
    }
    if (progmax > 0)
    {
        printf "Program: %2.1f%%, ", (text / progmax) * 100;
    }
    else
    {
        print ""
    }
    if (datamax > 0)
    {
        printf "RAM: %2.1f%%, ", (data / datamax) * 100;
    }
    else
    {
        print ""
    }
    if (eeprom > 0)
    {
        if (eeprommax > 0)
        {
            printf "EEPROM: %2.1f%%", (eeprom / eeprommax) * 100;
        }
        else
        {
            print ""
        }
    }
    print ""
}'
