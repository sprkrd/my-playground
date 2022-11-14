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

namespace {

typedef std::tuple<int,int> Point2i;

const char* kColorCodes[] = {"\033[32m", "\033[33m", "\033[31m"};
 
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

class MovableToken : public Fl_Box {
    public:

        MovableToken(int X, int Y, int W, int H, const char* l = 0) :
            Fl_Box(X, Y, W, H, l) {
            box(FL_FLAT_BOX);
            color(FL_RED);
        }

        int handle(int event) override { 
            switch (event) {
                case FL_PUSH: {
                    if (Fl::event_button() == FL_LEFT_MOUSE) {
                        //std::cout << "FL_PUSH" << std::endl;
                        offsetX = Fl::event_x() - x();
                        offsetY = Fl::event_y() - y();
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
                    parent()->redraw();
                    return 1;
                }
                case FL_RELEASE: {
                    //std::cout << "FL_RELEASE" << std::endl;
                    parent()->init_sizes();
                    parent()->redraw();
                    return 1;
                }
                default:
                    break;
            }
            return 0;
        }

    private:
        int offsetX;
        int offsetY;

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
            auto* p = Fl_Group::current();
            Fl_Tile* tile = new Fl_Tile(p->x(), p->y(), p->w(), p->h());
            //auto* limits = new Fl_Box(p->x(), p->y()+50, p->w(), p->h()-100);
            //limits->box(FL_FLAT_BOX);
            //limits->color(FL_RED);
            //tile->resizable(limits);
            InitMainView();
            InitLogTerminal();
            tile->end();
        }

        void InitMainView() {
            auto* p = Fl_Group::current();
            int mainViewHeight = kDefaultMainViewArea*p->h();
            pMainView = new Fl_Group(p->x(), p->y(), p->w(), mainViewHeight);
            pMainView->box(FL_DOWN_BOX);
            auto token1 = new MovableToken(50, 50, 50, 50);
            pMainView->end();
        }

        void InitLogTerminal() {
            auto* p = Fl_Group::current();
            int logTerminalY = pMainView->x() + pMainView->h();
            int logTerminalHeight = p->h() - pMainView->h();
            pLogTerminal = new Fl_Simple_Terminal(p->x(), logTerminalY, p->w(), logTerminalHeight);
            pLogTerminal->box(FL_DOWN_BOX);
            pLogTerminal->ansi(true);
            pLogTerminal->end();
        }

        std::unique_ptr<Fl_Window> pWin;
        Fl_Group* pMainView;
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

