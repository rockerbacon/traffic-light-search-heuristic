#!/bin/bash

SCRIPT_PATH=$(realpath $(dirname $0))
BUILD_PATH="${SCRIPT_PATH}/build"

if [ "$1" != "clean" ]; then
	mkdir -p "$BUILD_PATH"
	cd "$BUILD_PATH"
	cmake "$SCRIPT_PATH" -G "Unix Makefiles"

	if [ "$1" != "cmake-only" ]; then
		make $1
	fi
else
	rm -rf "$BUILD_PATH"
fi
