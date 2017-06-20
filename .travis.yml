sudo: false
language: cpp
compiler: g++
addons:
  apt:
    sources:
    - ubuntu-toolchain-r-test
    packages:
    - g++-4.8

install:
- if [ "$CXX" = "g++" ]; then export CXX="g++-4.8"; fi

script: "bash -c ./build.sh"
