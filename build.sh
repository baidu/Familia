#!/bin/bash

echo "Environment: `uname -a`"
echo "Compiler: `$CXX --version`"

mkdir -p third_party
make deps
make clean && make -j4;
