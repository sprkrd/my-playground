
include_directories(include)
set(BINARY_NAME Chapter03)
set(LIBRARIES SFML::Graphics SFML::Window SFML::Audio)
set(HEADER_FILES
    include/Game.hpp
    include/ResourceHolder.hpp
    include/ResourceManager.hpp
    include/Entity.hpp
)
set(SOURCE_FILES
    Game.cpp
    ResourceManager.cpp
    Entity.cpp
    ${BINARY_NAME}.cpp
)

add_application()

