
include_directories(include)
set(BINARY_NAME Chapter02)
set(LIBRARIES SFML::Graphics SFML::Window)
set(HEADER_FILES include/Game.hpp include/ResourceManager.hpp)
set(SOURCE_FILES Game.cpp ResourceManager.cpp ${BINARY_NAME}.cpp)

add_application()

