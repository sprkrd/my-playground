cmake_minimum_required(VERSION 3.25)
project(CMakeSFMLProject LANGUAGES CXX)

set(GLFW_VERSION 3.4)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(GLFW
    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG ${GLFW_VERSION}
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)

if (NOT GLFW_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(GLFW)
endif()

find_package(OpenGL REQUIRED COMPONENTS OpenGL)

include_directories(${OPENGL_INCLUDE_DIR})

add_executable(main src/main.cpp)
target_compile_features(main PRIVATE cxx_std_17)


target_link_libraries(main PRIVATE glfw OpenGL::GL)
