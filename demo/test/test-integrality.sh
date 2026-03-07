#!/bin/sh

# test-integrality.sh – Header files integrality test

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

# Define paths
INCLUDE_DIR="../../include"
CPP_DIR="./integrality"
CPP_FILE="$CPP_DIR/test-integrality.cpp"
BUILD_DIR="./build"
OBJ_FILE="$BUILD_DIR/integrality.o"

# 1. check if paths exist
[ -d "$INCLUDE_DIR" ] || error_exit "Include directory not found: $INCLUDE_DIR"
[ -d "$CPP_DIR" ]     || error_exit "Output directory not found: $CPP_DIR"
[ -d "$BUILD_DIR" ]   || error_exit "Build directory not found: $BUILD_DIR"

# 2. generate 'test-integrality.cpp'
> "$CPP_FILE" || error_exit "Cannot write to $CPP_FILE"

# 3. collect 'include/*.h'，exclude 'push.h' and 'pop.h'
count=0
for header in "$INCLUDE_DIR"/*.h; do
    [ -f "$header" ] || continue   # skip no match

    base=$(basename "$header")
    case "$base" in
        push.h|pop.h) continue ;;
    esac

    echo "#include \"$base\"" >> "$CPP_FILE" || error_exit "Failed to write to $CPP_FILE"
    count=$((count + 1))
done

if [ "$count" -eq 0 ]; then
    error_exit "No header files found to test (after exclusion)."
fi

# 4. compile to object
c++ -std=c++23 -c "$CPP_FILE" -o "$OBJ_FILE" -I"$INCLUDE_DIR" || error_exit "Failed to compile $CPP_FILE."

printf "Integrity test passed.\n"
exit 0
