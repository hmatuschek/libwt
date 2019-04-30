#!/usr/bin/env bash

git pull
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=~/.local -DENABLE_PYTHON=OFF -DBUILD_UNITTEST=OFF -DENABLE_OPENMP=ON -DCMAKE_BUILD_TYPE=RELEASE
make
make install
cd ..

