#!/bin/bash
set -ex
export CFLAGS='-g3'
export CXXFLAGS='-g3'
rm -rf build || true
meson setup build --prefix=/usr -Dresources=true -Dbackgrounds=$PWD/data/backgrounds "$@"
ninja -C build -j`nproc`
echo -e "run\nbacktrace\n" | gdb ./build/pardus-pen $ARGS
