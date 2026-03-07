#!/bin/sh
# run-all.sh – Run all test, entry point

cd "$(dirname "$0")" || exit 1
SCRIPT_DIR=$(pwd -P)
cd "$SCRIPT_DIR" || exit 1

echo "Running integrity test..."
./test-integrality.sh || exit 1

# compile unit test
./../acyclic/make.sh || exit 1
./../io/make.sh || exit 1
./../meta/make.sh || exit 1
./../print/make.sh || exit 1
./../tuple/make.sh || exit 1
./../view/make.sh || exit 1

# run unit test
./../acyclic/build/demo.out || exit 1
./../io/build/demo.out || exit 1
./../meta/build/demo.out || exit 1
./../print/build/demo.out || exit 1
./../tuple/build/demo.out || exit 1
./../view/build/demo.out || exit 1

echo "All tests passed."
exit 0
