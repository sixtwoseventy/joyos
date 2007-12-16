#!/bin/sh

# Make release archives

JOYOS_VER=`cat src/inc/kern/global.h | grep "VERSION" | cut -f 2 -d "\""`
HAPPY_VER=`cat src/inc/config.h | grep "VERSION_STRING" | cut -f 2 -d "\""`
SVN_VER=`svn info | grep "Revision" | cut -f 2 -d " "`

REL_NAME="joyos-$JOYOS_VER"
DOC_NAME="joyos-docs-$JOYOS_VER"

echo "-- Making distribtion dist/$REL_NAME"

mkdir -p dist

cd release
tar czf $REL_NAME.tar.gz 6.270
zip -rq $REL_NAME.zip 6.270
mv $REL_NAME.* ../dist

cd ../doc
tar czf $DOC_NAME.tar.gz api
mv $DOC_NAME.tar.gz ../dist
