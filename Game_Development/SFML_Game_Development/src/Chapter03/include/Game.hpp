#pragma once

#include "ResourceManager.hpp"
#include <SFML/Graphics.hpp>

constexpr float kPlayerSpeed = 500.f;
constexpr float kTimestep = 0.01;

class Game
{
    public:
        Game(int argc, char* argv[]);

        void run();

    private:

        void loadAssets();

        void shutdown();

        void processEvents();

		void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);
        
        void update(float deltaTime);

        void render();

        void renderUI();

    private:

        sf::RenderWindow mWindow;
        sf::Sprite mPlayer;

        ResourceManager mResourceManager;

        int mFps;
        bool mVsync;
		bool mIsMovingUp;
		bool mIsMovingDown;
		bool mIsMovingLeft;
		bool mIsMovingRight;
};
