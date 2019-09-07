#!/bin/sh
set -e

#local build directory
BUILD_DIR=build/linux
ARCHIVE_DIR=archive/linux

mkdir -p $BUILD_DIR
mkdir -p $ARCHIVE_DIR

mkdir -p $BUILD_DIR/Debug
cmake -B$BUILD_DIR/Debug -H. -G "Unix Makefiles"\
    -DCMAKE_BUILD_TYPE=Debug\
    -DCMAKE_INSTALL_PREFIX=$ARCHIVE_DIR/debug
make install -C $BUILD_DIR/Debug

mkdir -p $BUILD_DIR/Release
cmake -B$BUILD_DIR/Release -H. -G "Unix Makefiles"\
    -DCMAKE_BUILD_TYPE=Release\
    -DCMAKE_INSTALL_PREFIX=$ARCHIVE_DIR/release
make install -C $BUILD_DIR/Release