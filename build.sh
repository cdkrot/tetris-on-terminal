#!/bin/sh

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


CPP_SRCS=$(find ./src -name "*.cpp")
OBJS=""
for SRC in $CPP_SRCS
do
	echo "# source: $SRC"
	OUT=$(echo $SRC | sed -e "s:./src:./obj:" | sed -e "s:.cpp$:.o:")
	g++ -Wall -Wextra -g -c -std=c++11 $SRC -o $OUT
	if [ $? -ne 0 ]
	then
		echo "## Suspended build due to errors"
		exit 2
	fi
	OBJS="$OBJS $OUT"
done

echo "## Linking"

g++ $OBJS -lcurses -o ./bin/program

if [ $? -ne 0 ]
then
	echo "## Failed linking"
	exit 3
fi

echo "## DONE"
