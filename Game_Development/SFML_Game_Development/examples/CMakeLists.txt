link_libraries(SFML::Graphics SFML::Window)

function (add_example exampleName)
add_executable(${exampleName} ${exampleName}.cpp)
set_target_properties(${exampleName} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${EXAMPLES_OUTPUT_DIR}"
)
endfunction()

add_example(window_tutorial)
add_example(graphics_tutorial)
add_example(hello)

