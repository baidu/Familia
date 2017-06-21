#!/bin/bash

mkdir -p third_party
make deps
make clean && make -j4;
