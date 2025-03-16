// following the tutorial @ https://www.sfml-dev.org/tutorials/3.0/graphics/draw/#introduction

#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode({800,600}), "Graphics tutorial");

    bool vsyncEnabled = true;
    window.setVerticalSyncEnabled(vsyncEnabled);

    unsigned fps = 0;
    unsigned frameCount = 0;
    sf::Clock clock;

    sf::Font font("/usr/share/texmf/fonts/opentype/public/lm/lmmonolt10-oblique.otf");
    sf::Text fpsText(font);
    fpsText.setString(std::to_string(fps));
    fpsText.setCharacterSize(24);
    fpsText.setFillColor(sf::Color::White);

    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->code == sf::Keyboard::Key::V)
                {
                    vsyncEnabled = !vsyncEnabled;
                    window.setVerticalSyncEnabled(vsyncEnabled);
                }
            }
        }

        if (clock.getElapsedTime().asSeconds() >= 1.0)
        {
            fps = frameCount;
            frameCount = 0;
            clock.restart();
            fpsText.setString(std::to_string(fps));
        }

        window.clear(sf::Color::Black);

        // draw stuff
        window.draw(fpsText);

        ++frameCount;
        window.display();
        
    }
}
