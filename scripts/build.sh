#!/bin/sh

set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P)
REPO_ROOT=$(dirname "$SCRIPT_DIR")

BUILD_DIR=${BUILD_DIR:-"$REPO_ROOT/build"}
BUILD_TYPE=${BUILD_TYPE:-Release}
BUILD_TARGET=${BUILD_TARGET:-llama-ignite-npu}
JOBS=${JOBS:-$(getconf _NPROCESSORS_ONLN 2>/dev/null || nproc 2>/dev/null || echo 4)}

if [ ! -f "$REPO_ROOT/CMakeLists.txt" ]; then
    echo "error: could not find CMakeLists.txt in $REPO_ROOT" >&2
    exit 1
fi

cmake -S "$REPO_ROOT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" "$@"
cmake --build "$BUILD_DIR" --target "$BUILD_TARGET" --config "$BUILD_TYPE" -j"$JOBS"
