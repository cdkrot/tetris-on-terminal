#!/bin/sh
# configurable build script.
# configuration is done by build-DBG.sh and build-REL.sh


echo "## building"
if [ ! -d obj ]
then
	mkdir obj
	if [ $? -ne 0 ]
	then
		echo "## Failed to create ./obj directory"
		echo "## Suspended due to errors"
		exit 1
	fi
fi

if [ ! -d bin ]
then
	mkdir bin
	if [ $? -ne 0 ]
	then
		echo "## Failed to create ./bin directory"
		echo "## Suspended due to errors"
		exit 1
	fi
fi

echo "## generating src/buildconf.h"
./util/gen-buildconf.sh  $1> src/buildconf.h
echo "## compiling"

CPP_SRCS=$(find ./src -name "*.cpp")
OBJS=""
for SRC in $CPP_SRCS
do
	echo "# source: $SRC"
	OUT=$(echo $SRC | sed -e "s:./src:./obj:" | sed -e "s:.cpp$:.o:")
	
	if [ ! -d $(dirname $OUT) ]
	then
		mkdir -p $(dirname $OUT)
	fi
	
	g++ $CPP_FLAGS -c -I src $SRC -o $OUT
	if [ $? -ne 0 ]
	then
		echo "## Suspended build due to errors"
		exit 2
	fi
	OBJS="$OBJS $OUT"
done

echo "## Linking"

g++ $OBJS $LINK_FLAGS -o ./bin/tetris-on-terminal

if [ $? -ne 0 ]
then
	echo "## Failed linking"
	exit 3
fi

echo "## DONE"
