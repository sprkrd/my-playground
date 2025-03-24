macro(add_application)
    add_executable(${BINARY_NAME} ${HEADER_FILES} ${SOURCE_FILES})
    target_link_libraries(${BINARY_NAME} PRIVATE ${LIBRARIES})
    set_target_properties(${BINARY_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${APPLICATIONS_OUTPUT_DIRECTORY}"
            RUNTIME_OUTPUT_DIRECTORY_RELEASE "${APPLICATIONS_OUTPUT_DIRECTORY_RELEASE}"
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${APPLICATIONS_OUTPUT_DIRECTORY_DEBUG}"
    )
endmacro()

add_subdirectory(Chapter01)
add_subdirectory(Chapter02)

