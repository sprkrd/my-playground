link_libraries(SFML::Graphics SFML::Window)
#target_compile_features(main PRIVATE cxx_std_17)

function (add_example exampleName)
add_executable(${exampleName} ${exampleName}.cpp)
endfunction()

add_example(window_tutorial)
add_example(graphics_tutorial)
add_example(hello)

