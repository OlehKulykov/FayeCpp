#!/bin/sh
set -e

sudo apt-get update -qq
sudo apt-get install -qq cmake


# Clone all submodules
git submodule update --init --recursive

# Make build directories for qmake, and CMake in both shared and static mode
mkdir ../build

# Configure using CMake (shared)
cd ../build
cmake -DCMAKE_BUILD_TYPE=Release ../FayeCpp
