#include <SFML/Graphics.hpp>

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({640u,480u}), "CMake SFML Project");

    sf::CircleShape shape;
    shape.setRadius(40.f);
    shape.setPosition({100.f, 100.f});
    shape.setFillColor(sf::Color::Cyan);

    window.setFramerateLimit(144);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        window.clear();

        window.draw(shape);

        window.display();
    }
}
