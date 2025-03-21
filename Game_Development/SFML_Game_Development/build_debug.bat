cmake -DCMAKE_BUILD_TYPE=Debug -B build/Debug
cd build/Debug
MSBuild ALL_BUILD.vcxproj -property:Configuration=Debug /m
