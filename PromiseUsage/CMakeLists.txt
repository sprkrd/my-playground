cmake_minimum_required(VERSION 4.1)
project(PromiseUsage)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS circular_buffer)

set(SOURCES main.cpp)
set(HEADERS
        include/ash/SyncCircularBuffer.hpp
        include/ash/SyncCircularBuffer.ipp
        include/ash/Future.hpp
        include/ash/Future.ipp
        include/ash/SharedState.hpp
        include/ash/Promise.ipp
        include/ash/Promise.hpp
        include/ash/SharedState.ipp
        include/ash/Module.cpp
        include/ash/Module.hpp
)

add_executable(main ${SOURCES} ${HEADERS})

target_include_directories(main PRIVATE include)
target_link_libraries(main Boost::circular_buffer)
set_target_properties(main PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/$<IF:$<CONFIG:Debug>,debug,release>
)

function(enable_fake target fake)

endfunction()