#!/bin/sh
set -ex
export CFLAGS='-g3'
export CXXFLAGS='-g3'
export GSETTINGS_SCHEMAS_DIR=$PWD/data
glib-compile-schemas ./data
rm -rf build || true
meson setup build --prefix=/usr -Dresources=true
ninja -C build
echo -e gdb "run\nbacktrace\n" | ./build/pardus-pen
