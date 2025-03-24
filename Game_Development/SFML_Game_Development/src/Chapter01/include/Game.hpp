#include <SFML/Graphics.hpp>

constexpr float kPlayerSpeed = 500.f;
constexpr float kTimestep = 0.01;

class Game
{
    public:
        Game(int argc, char* argv[]);

        void run();

    private:

        void loadAssets(const char* binPath);

        void shutdown();

        void processEvents();

		void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);
        
        void update(float deltaTime);

        void render();

        void renderUI();

    private:

        sf::RenderWindow mWindow;
        sf::Texture mTexture;
        sf::Sprite mPlayer;

        std::filesystem::path mAssetsPath;

        sf::Font mFont;

        int mFps;
        bool mVsync;
		bool mIsMovingUp;
		bool mIsMovingDown;
		bool mIsMovingLeft;
		bool mIsMovingRight;
};
