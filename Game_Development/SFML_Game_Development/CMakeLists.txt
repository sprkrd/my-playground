cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
if (NOT SFML_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(SFML)
endif()

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)
add_subdirectory(examples)
