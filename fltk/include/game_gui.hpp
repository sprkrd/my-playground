#include <memory>
#include <string>
#include <vector>


constexpr int kBoardWidthTiles = 5;
constexpr int kBoardHeightTiles = 4;
constexpr int kNumberOfCells = kBoardWidthTiles*kBoardHeightTiles;
constexpr int kTileSize = 65;
constexpr int kCellMargin = 15;
constexpr int kDefaultWindowWidth = 800;
constexpr int kDefaultWindowHeight = 640;
constexpr int kDefaultTerminalHeight = 100;

typedef std::vector<std::string> TokenArray;

class GameGUI {
    public:

        enum LogLevel { INFO = 0, WARN, ERROR };

        GameGUI(bool editable = true);

        void ResetBoardStatus(const TokenArray& tokens);

        void ResetTokensOutOfBoard();

        void ResetTokensOutOfBoard(const TokenArray& tokens);

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

