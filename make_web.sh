#!/bin/sh
set -e

mkdir -p data
cp -rf res data

EMSDK=$(printenv EMSDK)
PATH=$EMSDK/fastcomp/emscripten:$PATH
TOOLCHAIN=$EMSDK/fastcomp/emscripten/cmake/Modules/Platform/Emscripten.cmake

BUILD_DIR=build/web
ARCHIVE_DIR=archive/web
VENDOR_DIR=$(printenv VENDOR_HOME)

mkdir -p $BUILD_DIR
mkdir -p $ARCHIVE_DIR

mkdir -p $BUILD_DIR/Debug
cmake   -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN\
        -B$BUILD_DIR/Debug\
        -H. -G "Unix Makefiles"\
        -DCMAKE_BUILD_TYPE=Debug\
        -DCMAKE_INSTALL_PREFIX=$ARCHIVE_DIR/debug\
        -DVENDOR_DIRECTORY=$VENDOR_DIR\
        -DVENDOR_LIBRARY=$VENDOR_DIR/build/web/debug\
        -DUSE_OPENGL_2=ON
make install -C $BUILD_DIR/Debug

mkdir -p $BUILD_DIR/Release
cmake   -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN\
        -B$BUILD_DIR/Release\
        -H. -G "Unix Makefiles"\
        -DCMAKE_BUILD_TYPE=Release\
        -DCMAKE_INSTALL_PREFIX=$ARCHIVE_DIR/release\
        -DVENDOR_DIRECTORY=$VENDOR_DIR\
        -DVENDOR_LIBRARY=$VENDOR_DIR/build/web/release\
        -DUSE_OPENGL_2=ON
make install -C $BUILD_DIR/Release

#clear temp data
rm -rf data