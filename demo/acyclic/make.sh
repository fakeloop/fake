#!/bin/sh

# make.sh – Acyclic

set -u

# Error report color
YELLOW='\033[33m'
RESET='\033[0m'

# Error report
error_exit() {
    printf "${YELLOW}Error: %s${RESET}\n" "$1" >&2
    exit 1
}

# Set path
cd "$(dirname "$0")" || error_exit "Cannot cd to script directory."
SCRIPT_DIR=$(pwd -P)
cd "$SCRIPT_DIR" || error_exit "Cannot determine script directory."

CPP_DIR="."
INCLUDE_DIR="../../include"
BUILD_DIR="./build"
CPP_FILE="$CPP_DIR/test-integrality.cpp"
TARGET_DIR="$BUILD_DIR/demo.out"

# Check if paths exist
[ -d "$CPP_DIR" ]     || error_exit "Source directory not found: $CPP_DIR"
[ -d "$INCLUDE_DIR" ] || error_exit "Include directory not found: $INCLUDE_DIR"
[ -d "$BUILD_DIR" ]   || error_exit "Build directory not found: $BUILD_DIR"

# Build
list="aspect;awaited;awaiter;branch;coroutine;demo;exception;nested;sequence;simple;"

compile() {
	c++ "$CPP_DIR/$1.cpp" -c -o "$BUILD_DIR/$1.o" -iquote "$INCLUDE_DIR" -std=c++23 || error_exit "Failed to compile $1"
}

concat() {
	echo "$list" | while read -r -d ';' result; do printf "$BUILD_DIR/$result.o "; done
}

make() {
	echo "$list" | while read -r -d ';' result; do compile "$result"; done
}

link() {
	c++ $(concat) -o "$TARGET_DIR" -pthread || error_exit "Failed to link acyclic demo"
}

make
link

exit 0
