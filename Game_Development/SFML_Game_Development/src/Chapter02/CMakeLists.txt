
include_directories(include)
set(BINARY_NAME Chapter02)
set(LIBRARIES SFML::Graphics SFML::Window)
set(HEADER_FILES include/Game.hpp)
set(SOURCE_FILES Game.cpp ${BINARY_NAME}.cpp)


add_executable(${BINARY_NAME} ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(${BINARY_NAME} PRIVATE ${LIBRARIES})
set_target_properties(${BINARY_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${APPLICATIONS_OUTPUT_DIR}"
)

