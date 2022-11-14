cmake_minimum_required(VERSION 3.2.3)

project(fltk_playground)

# The following line is required only if (a) you didn't install FLTK
# or if (b) find_package can't find your installation directory because
# you installed FLTK in a non-standard location.  It points to
# (a) the base folder of the build directory, or
# (b) <fltk-install-prefix>/share/fltk
# resp., where <fltk-install-prefix> is the installation prefix you
# used to install FLTK.
# (The file FLTKConfig.cmake and others must be found in that path.)

# set(FLTK_DIR /path/to/fltk)

find_package(FLTK REQUIRED NO_MODULE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
message("Executables dir: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

include_directories(include ${FLTK_INCLUDE_DIRS})

add_subdirectory(src)
