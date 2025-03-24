
include_directories(include)
set(BINARY_NAME Chapter01)
set(LIBRARIES SFML::Graphics SFML::Window)
set(HEADER_FILES include/Game.hpp)
set(SOURCE_FILES Game.cpp ${BINARY_NAME}.cpp)


add_application()



