#!/bin/bash

# Get the root path (the current script directory)
root_path=$(dirname "$0")

cd "$root_path"

# Check if build type is specified
if [ -z "$1" ]; then
    echo "Build type not specified, using Release"
    build_type="Release"
else
    build_type="$1"
fi

# Check if the build type is valid
if [[ "$build_type" != "Release" && "$build_type" != "Debug" ]]; then
    echo "Invalid build type $build_type, only Release and Debug are allowed"
    exit 1
fi

# Check if the build directory exists, if not, run cmake
if [ -d "build/$build_type" ]; then
    echo "build/$build_type already exists, skipping call to cmake"
else
    cmake . -DCMAKE_BUILD_TYPE="$build_type" -B "build/$build_type"
fi

cd "build/$build_type"
make all -j

cp compile_commands.json ../..
