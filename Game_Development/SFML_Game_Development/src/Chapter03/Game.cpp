#include "Game.hpp"

#include <filesystem>

namespace
{

const sf::Texture kNullTexture;

}

Game::Game(int argc, char* argv[])
: mWindow(sf::VideoMode({640,480}), "SFML Application")
, mPlayer(kNullTexture)
, mResourceManager(ResourceManager::fromExePath(argv[0]))
, mFps(0)
, mVsync(true)
, mIsMovingUp(false)
, mIsMovingDown(false)
, mIsMovingLeft(false)
, mIsMovingRight(false)
{
    mWindow.setVerticalSyncEnabled(mVsync);

    loadAssets();

    mPlayer.setPosition({100.f, 100.f});
    // mPlayer.setRadius(40.f);
    // mPlayer.setColor(sf::Color::Cyan);
}

void Game::run()
{
    int fpsCount = 0;
    const sf::Time sfTimestep = sf::seconds(kTimestep);
    const sf::Time fpsInterval = sf::seconds(1.f);

    sf::Clock clock, fpsClock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    while (mWindow.isOpen())
    {
        processEvents();
        timeSinceLastUpdate += clock.restart();
        while (timeSinceLastUpdate >= sfTimestep)
        {
            timeSinceLastUpdate -= sfTimestep;
            // processEvents();
            update(kTimestep);
        }

        if (fpsClock.getElapsedTime() >= fpsInterval)
        {
            mFps = fpsCount;
            fpsCount = 0;
            fpsClock.restart();
        }

        render();
        ++fpsCount;
    }
}

void Game::loadAssets()
{
    mResourceManager.load<sf::Texture>(TextureId::Airplane, "Eagle.png");
    mResourceManager.load<sf::Font>(FontId::GameOver, "game_over.ttf");

    mPlayer.setTexture(mResourceManager.get<sf::Texture>(TextureId::Airplane), true);
}

void Game::shutdown()
{
    mWindow.close();
}

void Game::processEvents()
{
	mWindow.handleEvents(
		[this](const sf::Event::Closed&)
		{
            shutdown();
		},
		
		[this](const sf::Event::KeyPressed& keyPressed)
		{
			handlePlayerInput(keyPressed.code, true);
		},
		
		[this](const sf::Event::KeyReleased& keyReleased)
		{
			handlePlayerInput(keyReleased.code, false);
		},
		
		[this](const sf::Event::Resized& resized)
		{
			sf::View visibleArea({0.f,0.f}, sf::Vector2f(resized.size));
			mWindow.setView(visibleArea);
		}
	);
}

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{
	switch (key)
	{
		case sf::Keyboard::Key::W:
			mIsMovingUp = isPressed;
			break;
		case sf::Keyboard::Key::S:
			mIsMovingDown = isPressed;
			break;
		case sf::Keyboard::Key::A:
			mIsMovingLeft = isPressed;
			break;
		case sf::Keyboard::Key::D:
			mIsMovingRight = isPressed;
			break;
		case sf::Keyboard::Key::Escape:
            shutdown();
            break;
        case sf::Keyboard::Key::V:
            if (isPressed)
            {
                mVsync = !mVsync;
                mWindow.setVerticalSyncEnabled(mVsync);
            }
            break;
		default:
			break;
	}
}

void Game::update(float deltaTime)
{
	sf::Vector2f movement(0, 0);
	if (mIsMovingUp)
        movement.y -= kPlayerSpeed;
    if (mIsMovingDown)
        movement.y += kPlayerSpeed;
	if (mIsMovingLeft)
        movement.x -= kPlayerSpeed;
	if (mIsMovingRight)
        movement.x += kPlayerSpeed;
    mPlayer.move(movement*deltaTime);
}

void Game::render()
{
    mWindow.clear();

    mWindow.draw(mPlayer);
    renderUI();

    mWindow.display();
}

void Game::renderUI()
{
    auto currentView = mWindow.getView();
    sf::View uiView(sf::FloatRect({0.f, 0.f}, sf::Vector2f(mWindow.getSize())));
    mWindow.setView(uiView);
    const auto& font = mResourceManager.get<sf::Font>(FontId::GameOver);
    sf::Text fpsText(font, "FPS: " + std::to_string(mFps));
    fpsText.setCharacterSize(48);
    fpsText.setFillColor(sf::Color::White);
    mWindow.draw(fpsText);
    mWindow.setView(currentView);
}
