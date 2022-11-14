#include <memory>
#include <string>
#include <vector>


constexpr int kBoardWidthTiles = 5;
constexpr int kBoardHeightTiles = 4;
constexpr int kTileSize = 50;
constexpr int kCellMargin = 20;
constexpr int kDefaultWindowWidth = 800;
constexpr int kDefaultWindowHeight = 640;
constexpr double kDefaultMainViewArea = 0.8;

typedef std::vector<std::string> TokenArray;

class GameGUI {
    public:

        enum LogLevel { INFO = 0, WARN, ERROR };

        GameGUI();

        void SetBoardStatus(const TokenArray& tokens);

        void SetTokensOutOfBoard(const TokenArray& tokens);

        void Log(const std::string& message, LogLevel logLevel);

        TokenArray GetBoardStatus() const;

        TokenArray GetTokensOutOfBoard() const;

        void Close();

        int Run(int argc, char* argv[]);

        ~GameGUI();

    private:
        class Impl;
        std::unique_ptr<Impl> pImpl;
};

