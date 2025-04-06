#pragma once

#include "ResourceManager.hpp"
#include "Entity.hpp"

#include <SFML/Graphics.hpp>

constexpr float kPlayerSpeed = 500.f;
constexpr float kTimestep = 0.01;

class Game
{
    public:
        Game(int argc, char* argv[]);

        void run();

    private:

        void loadAssets(const char* exePath);

        void shutdown();

        void processEvents();

		void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);
        
        void update(float deltaTime);

        void render();

        void renderUI();

    private:

        sf::RenderWindow mWindow;

        Entity::Ptr mPlayer;

        int mFps;
        bool mVsync;
		bool mIsMovingUp;
		bool mIsMovingDown;
		bool mIsMovingLeft;
		bool mIsMovingRight;
};
