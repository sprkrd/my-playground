cmake -DCMAKE_BUILD_TYPE=Release -B build/Release
cd build/Release
MSBuild ALL_BUILD.vcxproj -property:Configuration=Release /m
