#!/bin/sh

# Print out a JoyOS/Happyboard version string for Doxygen

JOYOS_VER=`cat src/inc/kern/global.h | grep "VERSION" | cut -f 2 -d "\""`
HAPPY_VER=`cat src/inc/config.h | grep "VERSION_STRING" | cut -f 2 -d "\""`
SVN_VER=`svn info | grep "Revision" | cut -f 2 -d " "`

VER_STRING="v$JOYOS_VER (b$SVN_VER/$HAPPY_VER)"
echo "PROJECT_NUMBER=\"$VER_STRING\""
