#!/bin/bash

installDir="/home/gregork/workspace/tracker/install"

mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$installDir ../src &&

make -j $nproc &&
make install
