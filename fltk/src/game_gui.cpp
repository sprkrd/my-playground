#include "game_gui.hpp"

#include <chrono>
#include <ctime>
#include <iostream>
#include <tuple>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Simple_Terminal.H>
#include <FL/Fl_Scroll.H>

namespace {

typedef std::tuple<int,int> Point2i;

const char* kColorCodes[] = {"\033[32m", "\033[33m", "\033[31m"};

constexpr Fl_Color kTokenColor = FL_MAGENTA;
 
std::string DateAndTime() {
    char buff[80];
    time_t rawTime = time(nullptr);
    struct tm* timeInfo = localtime(&rawTime);
    strftime(buff, 80, "%a %b %d %T", timeInfo);
    return std::string(buff);
}

int AlignCenter(int parentSize, int size) {
    return std::max((parentSize-size)/2, 0);
}

Point2i AlignCenter(int parentW, int parentH, int w, int h) {
    return Point2i(AlignCenter(parentW, w), AlignCenter(parentH, h));
}

class CenteredGroup : public Fl_Group {
    public:
        CenteredGroup(int W, int H, const char* l = nullptr) : Fl_Group(0, 0, W, H, l) {
            auto[X,Y] = AlignCenter(parent()->w(), parent()->h(), W, H);
            position(X, Y);
        }

        void resize(int X, int Y, int W, int H) override {
            int W0 = GetInitialWidth();
            int H0 = GetInitialHeight();
            //W = std::max(W, W0);
            //H = std::max(H, H0);
            if (W*H0 > H*W0)
                W = W0 * H/H0;
            else
                H = H0 * W/W0;
            std::tie(X, Y) = AlignCenter(parent()->w(), parent()->h(), W, H);
            Fl_Group::resize(X, Y, W, H);
        }

    private:

        int GetInitialWidth() {
            int* s = sizes();
            return s[1] - s[0];
        }

        int GetInitialHeight() {
            int* s = sizes();
            return s[3] - s[2];
        }
};

class FlLock {
    public:
        FlLock() {
            Fl::lock();
        }

        ~FlLock() {
            Fl::unlock();
            Fl::awake();
        }
};

void Clip(int& value, int minValue, int maxValue) {
    if (value < minValue)
        value = minValue;
    if (value > maxValue)
        value = maxValue;
}

bool IsBetween(int value, int minValue, int maxValue) {
    return minValue <= value && value <= maxValue;
}

bool IsWithin(int x, int y, const Fl_Widget* w) {
    return IsBetween(x, w->x(), w->x() + w->w()-1) &&
           IsBetween(y, w->y(), w->y() + w->h()-1);
}

class MovableToken : public Fl_Box {
    public:

        MovableToken(int X, int Y, int size, const char* l = 0) :
            Fl_Box(X, Y, size, size, l) {
            box(FL_OFLAT_BOX);
            ResetColor();
        }

        void ResetColor() {
            color(kTokenColor);
            labelcolor(fl_contrast(FL_WHITE, kTokenColor));
            redraw();
        }

        void Highlight() {
            color(fl_color_average(kTokenColor, FL_WHITE, 0.7));
            labelcolor(fl_contrast(FL_WHITE, color()));
            redraw();
        }

        int handle(int event) override { 
            switch (event) {
                case FL_ENTER: {
                    //std::cout << "FL_ENTER" << std::endl;
                    Highlight();
                    return 1;
                }
                case FL_LEAVE: {
                    //std::cout << "FL_LEAVE" << std::endl;
                    ResetColor();
                    return 1;
                }
                case FL_PUSH: {
                    if (Fl::event_button() == FL_LEFT_MOUSE) {
                        //std::cout << "FL_PUSH" << std::endl;
                        offsetX = Fl::event_x() - x();
                        offsetY = Fl::event_y() - y();
                        TopLevel();
                        return 1;
                    }
                    break;
                }
                case FL_DRAG: {
                    //std::cout << "FL_DRAG" << std::endl;
                    int parentBorderWidthX = Fl::box_dx(parent()->box());
                    int parentBorderWidthY = Fl::box_dy(parent()->box());
                    int newX = Fl::event_x() - offsetX;
                    int newY = Fl::event_y() - offsetY;
                    Clip(newX,
                            parent()->x()+parentBorderWidthX,
                            parent()->x()+parent()->w()-parentBorderWidthX-w());
                    Clip(newY,
                            parent()->y()+parentBorderWidthY,
                            parent()->y()+parent()->h()-parentBorderWidthY-h());
                    position(newX, newY);
                    RedrawParentWithBox();
                    return 1;
                }
                case FL_RELEASE: {
                    //std::cout << "FL_RELEASE" << std::endl;
                    parent()->init_sizes();
                    RedrawParentWithBox();
                    return 1;
                }
                default:
                    break;
            }
            return 0;
        }

    private:

        void RedrawParentWithBox() {
            auto* p = parent();
            while (p->box() == FL_NO_BOX && p->parent())
                p = p->parent();
            p->redraw();
        }

        void TopLevel() {
            parent()->insert(*this, parent()->children());
        }

        int offsetX;
        int offsetY;

};

class Board {
    public:
        void Init() {
            int boardWidth = (kCellMargin+kTileSize)*kBoardWidthTiles + kCellMargin;
            int boardHeight = (kCellMargin+kTileSize)*kBoardHeightTiles + kCellMargin;
            pBoard = new CenteredGroup(boardWidth, boardHeight);
            pBoard->box(FL_FLAT_BOX);
            pBoard->color(FL_BLACK);
            pBoard->end();
        }

        int TileSize() const {
            return pCells[0]->w();
        }

        int CellGap() const {
            return pCells[0]->x() - pBoard->x();
        }

        Fl_Box* At(int i, int j) {
            const auto* const_this = this;
            return const_cast<Fl_Box*>(const_this->At(i,j));
        }

        const Fl_Box* At(int i, int j) const {
            return pCells[i*kBoardWidthTiles + j];
        }

        const Fl_Box* Snap(int x, int y) const {
            const Fl_Box* cell = nullptr;
            if (IsWithin(x, y, pBoard)) {
                int s = TileSize();
                int g = CellGap();

                int i = (y - pBoard->y() - g)/(s+g);
                int j = (x - pBoard->x() - g)/(s+g);

                cell = At(i, j);
                if (!IsWithin(x, y, cell))
                    cell = nullptr;
            }
            return cell;
        }

        Fl_Box* Snap(int x, int y) {
            const auto* const_this = this;
            return const_cast<Fl_Box*>(const_this->Snap(x, y));
        }

    private:

        Fl_Group* pBoard;
        std::vector<Fl_Box*> pCells;

};

} // anonymous ns

class GameGUI::Impl {
    public:
        Impl() {
            pWin = std::make_unique<Fl_Window>(kDefaultWindowWidth, kDefaultWindowHeight);
            InitSplitWindow();
            pWin->end();
            pWin->resizable(pWin.get());
            Fl::lock();
        }

        void SetBoardStatus(const TokenArray& tokens) {
        }

        void SetTokensOutOfBoard(const TokenArray& tokens) {
        }

        void Log(const std::string& message, LogLevel LogLevel) {
            FlLock lock;
            std::string dateAndTime = DateAndTime();
            const char* colorCode = kColorCodes[static_cast<int>(LogLevel)];
            pLogTerminal->printf("%s: %s%s\033[0m\n", dateAndTime.c_str(), colorCode, message.c_str());
        }

        TokenArray GetBoardStatus() const {
            return TokenArray();
        }

        TokenArray GetTokensOutOfBoard() const {
            return TokenArray();
        }

        void Close() {
            Fl::awake(&Impl::Close, this);
        }

        int Run(int argc, char* argv[]) {
            pWin->show(argc, argv);
            return Fl::run();
        }

    private:
        static void Close(void* data) {
            Impl* impl = static_cast<Impl*>(data);
            impl->pWin->hide();
        }

        void InitSplitWindow() {
            constexpr int minimumHeight = 100;
            auto* p = Fl_Group::current();
            Fl_Tile* tile = new Fl_Tile(p->x(), p->y(), p->w(), p->h());
            auto* limits = new Fl_Box(p->x(), p->y()+minimumHeight, p->w(), p->h()-2*minimumHeight);
            tile->resizable(limits);
            InitMainView();
            InitLogTerminal();
            tile->end();
        }

        void InitMainView() {
            auto* p = Fl_Group::current();
            int topTileHeight = kDefaultMainViewArea*p->h();
            auto* topTile = new Fl_Group(p->x(), p->y(), p->w(), topTileHeight);
            topTile->box(FL_DOWN_BOX);
            pMainView = new CenteredGroup(p->w(), topTileHeight);

            new MovableToken(50, 50, 50, "Hey!");

            pBoard.Init();

            pMainView->end();
            topTile->end();
        }

        void InitLogTerminal() {
            auto* p = Fl_Group::current();
            auto* lastChild = p->child(p->children()-1);
            int logTerminalY = lastChild->x() + lastChild->h();
            int logTerminalHeight = p->h() - lastChild->h();
            pLogTerminal = new Fl_Simple_Terminal(p->x(), logTerminalY, p->w(), logTerminalHeight);
            pLogTerminal->box(FL_DOWN_BOX);
            pLogTerminal->ansi(true);
            pLogTerminal->end();
        }

        std::unique_ptr<Fl_Window> pWin;
        Fl_Group* pMainView;
        Board pBoard;
        Fl_Simple_Terminal* pLogTerminal;
        
};


GameGUI::GameGUI() : pImpl(new Impl) {

}

void GameGUI::SetBoardStatus(const TokenArray& tokens) {
    pImpl->SetBoardStatus(tokens);
}

void GameGUI::SetTokensOutOfBoard(const TokenArray& tokens) {
    pImpl->SetTokensOutOfBoard(tokens);
}

void GameGUI::Log(const std::string& message, LogLevel LogLevel) {
    pImpl->Log(message, LogLevel);
}

TokenArray GameGUI::GetBoardStatus() const {
    return pImpl->GetBoardStatus();
}

TokenArray GameGUI::GetTokensOutOfBoard() const {
    return pImpl->GetTokensOutOfBoard();
}

void GameGUI::Close() {
    pImpl->Close();
}

int GameGUI::Run(int argc, char* argv[]) {
    return pImpl->Run(argc, argv);
}

GameGUI::~GameGUI() = default;

