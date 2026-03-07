#!/bin/sh

# make.sh – Demo

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
SOURCE_DIR="$CPP_DIR/demo.cpp"
TARGET_DIR="$BUILD_DIR/demo.out"

# Check if paths exist
[ -d "$CPP_DIR" ]     || error_exit "Output directory not found: $CPP_DIR"
[ -d "$INCLUDE_DIR" ] || error_exit "Include directory not found: $INCLUDE_DIR"
[ -d "$BUILD_DIR" ]   || error_exit "Build directory not found: $BUILD_DIR"

# Build
c++ "$SOURCE_DIR" -o "$TARGET_DIR" -iquote "$INCLUDE_DIR" -std=c++23 || error_exit "Failed to compile $SOURCE_DIR"

exit 0
