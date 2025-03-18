!/bin/bash

cmake -DCMAKE_BUILD_TYPE=Release -B build/Release
make -C build/Release -j
