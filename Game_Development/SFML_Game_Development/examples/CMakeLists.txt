link_libraries(SFML::Graphics)

function (add_example exampleName)
add_executable(${exampleName} ${exampleName}.cpp)
endfunction()

add_example(window_tutorial)
add_example(graphics_tutorial)

