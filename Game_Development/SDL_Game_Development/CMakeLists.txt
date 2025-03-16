cmake_minimum_required(VERSION 3.25)
project(CMakeSFMLProject LANGUAGES CXX)

set(SDL_VERSION 3.2.8)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-${SDL_VERSION}
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)

if (NOT SDL3_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(SDL3)
endif()


add_executable(main src/main.cpp)
target_compile_features(main PRIVATE cxx_std_17)


target_link_libraries(main PRIVATE SDL3::SDL3)
