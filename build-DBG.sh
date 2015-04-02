#!/bin/sh

export CPP_FLAGS=" -Wall -Wextra -g -std=c++11 -DDEBUG "
export LINK_FLAGS=" -lcurses "

./util/build.sh $(cat version.txt)
