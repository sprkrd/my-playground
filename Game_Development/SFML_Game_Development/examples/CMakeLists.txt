link_libraries(SFML::Graphics SFML::Window)

function (add_example exampleName)
add_executable(${exampleName} ${exampleName}.cpp)
set_target_properties(${exampleName} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${EXAMPLES_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${EXAMPLES_OUTPUT_DIRECTORY_RELEASE}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${EXAMPLES_OUTPUT_DIRECTORY_DEBUG}"
)
endfunction()

add_example(window_tutorial)
add_example(graphics_tutorial)
add_example(hello)

