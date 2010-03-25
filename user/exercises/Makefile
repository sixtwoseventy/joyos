##### JoyOS User Makefile ####

# Name of this project (no spaces or extension)
TARGET = exercise1a

# Source files - list all your .c files here (separated by spaces)
SRC = exercise1a.c

# USB-Serial port number for programming the happyboard

PORT = /dev/ttyUSB0

## Windows ##
# On Windows the Happyboard will show up as a serial port named COM*.
# The correct COM port number can be determined in the Device Manager.
# Right click on ”My Computer”, click ”Properties”, then ”Hardware”,
# then ”Device Manager”. Under the ”Ports (COM & LPT)” section, the
# last COM port should be the Happyboard.

## MAC OS X ##
# On Mac OS X the Happyboard will show up as a serial port at /dev/tty.usb**
# or /dev/tty.usbserial-*. To find the correct port number, plug the
# happyboard into a USB port and turn it on. After a few seconds run the
# following command in a terminal: This should display the correct port name.
#
# ls /dev/tty.usb*
#
# This should display the correct port name

## Linux ##
# On most linux distributions the Happyboard will show up as a serial port
# at /dev/ttyUSB** or /dev/ttyS**. A few seconds after turning on the
# HappyBoard, run `dmesg|tail` in a terminal. This should display the
# detection on driver setup messages for the Happyboard serial port. Look
# through the messages to identify the port associated with the HappyBoard.


# Top level JoyOS directory
TOPDIR = ../../

# Include support makefile
include ../makedefaults.inc
