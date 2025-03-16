// follows the tutorial @ https://www.sfml-dev.org/tutorials/3.0/window/window/

#include <SFML/Window.hpp>

#include <iostream>

int main()
{
    sf::Window window(sf::VideoMode({800, 600}), "My window");

    // also you can create the window later:
    // sf::Window window;
    // window.create(... same parameters as ctor)
    
    // playing with the window a bit

    window.setVerticalSyncEnabled(true);

    window.setPosition({10, 50});
    window.setSize({640,480});
    window.setTitle("SFML window");
    sf::Vector2u size = window.getSize();
    auto[width, height] = size;
    std::cout << width << " x " << height << std::endl;

    bool focus = window.hasFocus();

    std::cout << "Focus: " << focus << std::endl;
    
    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {

            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }
    }
}
