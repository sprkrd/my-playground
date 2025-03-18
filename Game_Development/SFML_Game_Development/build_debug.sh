!/bin/bash

cmake -DCMAKE_BUILD_TYPE=Debug -B build/Debug
make -C build/Debug -j
