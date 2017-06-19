#!/bin/bash

echo "mkdir"
mkdir -p third_party
echo "make deps"
make deps
echo "make -j4"
make clean && make -j4;
