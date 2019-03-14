#!/bin/bash
CMAKE=cmake

# clean build
./clean.sh

rm -f ./bin/dlocktest

# cmake -> Makefile -> Make
${CMAKE} ./ -DCMAKE_BUILD_TYPE=Release
make -j `nproc`

# clean generated files
./clean.sh
