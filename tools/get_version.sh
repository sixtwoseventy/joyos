#!/bin/sh

# Print out a JoyOS version string for Doxygen

JOYOS_VER=`cat src/inc/kern/global.h | grep "VERSION" | cut -f 2 -d "\""`
SVN_VER=`svn info | grep "Revision" | cut -f 2 -d " "`

VER_STRING="Revision $SVN_VER"
echo "PROJECT_NUMBER=\"$VER_STRING\""
