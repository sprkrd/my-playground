#include "game_gui.hpp"

#include <chrono>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <tuple>

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Simple_Terminal.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Tile.H>
#include <FL/names.h>

namespace {

typedef std::tuple<int,int> Point2i;

const char* kColorCodes[] = {"\033[32m", "\033[33m", "\033[31m"};

const Fl_Color kTokenColor  = FL_RED;
const Fl_Color kGoalColor   = fl_rgb_color(150, 150, 150);
const Fl_Color kCellColor   = fl_rgb_color(  0, 100, 210);
 
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

class Tile : public Fl_Box {
    public:
        Tile(int X, int Y, int size = kTileSize) : Fl_Box(X, Y, size, size) {
            box(FL_OFLAT_BOX);
            pInitialColor = color();
        }

        void ResetColor(Fl_Color color) {
            pInitialColor = color;
            ResetColor();
        }

        void ResetColor() {
            color(pInitialColor);
            labelcolor(fl_contrast(FL_WHITE, pInitialColor));
            redraw();
        }

        void Highlight() {
            Fl_Box::color(fl_color_average(pInitialColor, FL_WHITE, 0.75));
            labelcolor(fl_contrast(FL_WHITE, color()));
            redraw();
        }

    private:

        Fl_Color pInitialColor;

};

class Cell;

class Token : public Tile {
    public:
        Token(int X, int Y, int size = kTileSize, const std::string& l = "") :
            Tile(X, Y, size),
            pCellAt(nullptr) {
            box(FL_OFLAT_BOX);
            copy_label(l.c_str());
            ResetColor(kTokenColor);
        }

        Cell* At() const {
            return pCellAt;
        }

        void At(Cell* at) {
            pCellAt = at;
        }

        Point2i Center() const {
            return {x() + w()/2, y() + w()/2};
        }

    private:
        Cell* pCellAt;
};

class Cell : public Tile {
    public:
        enum CellType { NORMAL, GOAL };

        Cell(int X, int Y, CellType type = NORMAL) : Tile(X, Y), pContent(nullptr) {
            ResetColor(type == NORMAL? kCellColor : kGoalColor);
        }

        bool Empty() const {
            return !pContent;
        }

        void Content(Token* tok) {
            pContent = tok;
        }

        Token* Content() const {
            return pContent;
        }

    private:
        Token* pContent;
};

class GameArea : public Fl_Group {
    public:
        GameArea(bool editable = true) :
            Fl_Group(Fl_Group::current()->x(), Fl_Group::current()->y(),
                     Fl_Group::current()->w(), Fl_Group::current()->h()),
            pCells(kNumberOfCells, nullptr),
            pHighlightedToken(nullptr),
            pHighlightedCell(nullptr),
            pDraggedToken(nullptr),
            pDragOffsetX(0),
            pDragOffsetY(0),
            pEditable(editable) {
            InitBoard();
            InitOutOfBoardView();
        }

        void resize(int X, int Y, int W, int H) override {
            //W = std::max(W, pW0);
            //H = std::max(H, pH0);
            //if (W*pH0 > H*pW0)
                //W = pW0 * H/pH0;
            //else
                //H = pH0 * W/pW0;
            std::tie(X, Y) = AlignCenter(W, H, w(), h());
            Fl_Group::resize(X, Y, w(), h());
        }

        int handle(int event) override {
            return pEditable? HandleEditable(event) : Fl_Group::handle(event);
        }

        TokenArray GetBoardStatus() const {
            TokenArray array(pCells.size());
            for (size_t i = 0; i < pCells.size(); ++i) {
                if (Token* tok = pCells[i]->Content())
                    array[i] = std::string(tok->label());
            }
            return array;
        }

        TokenArray GetTokensOutOfBoard() const {
            TokenArray array(pOutOfBoardView->children());
            for (size_t i = 0; i < pOutOfBoardView->children(); ++i) {
                auto* child = pOutOfBoardView->child(i);
                array[i] = std::string(child->label());
            }
            return array;
        }

        void ResetBoardStatus(const TokenArray& array) {
            if (array.size() != pCells.size())
                throw std::invalid_argument("Mismatching array size");
            auto* previousGroup = Fl_Group::current();
            Fl_Group::current(this);
            for (size_t i = 0; i < pCells.size(); ++i) {
                if (Token* tok = pCells[i]->Content()) {
                    Fl::delete_widget(tok);
                    pCells[i]->Content(nullptr);
                }
                if (!array[i].empty()) {
                    std::cout << i << ' ' << array[i] << std::endl;
                    Token* tok = new Token(0, 0, kTileSize, array[i]);
                    MoveToken(tok, pCells[i]);
                }
            }
            Fl_Group::current(previousGroup);
        }

        void ResetTokensOutOfBoard() {
            pOutOfBoardView->clear();
            pOutOfBoardView->redraw();
        }

        void ResetTokensOutOfBoard(const TokenArray& array) {
            ResetTokensOutOfBoard();
            auto* previousGroup = Fl_Group::current();
            Fl_Group::current(pOutOfBoardView);
            for (const auto& label : array) {
                new Token(0, 0, pOutOfBoardView->h(), label);
            }
            Fl_Group::current(previousGroup);
        }

    private:

        int HandleEditable(int event) {
            auto* tok = dynamic_cast<Token*>(Fl::belowmouse());
            switch (event) {
                case FL_ENTER:
                case FL_MOVE: {
                    if (ChangeHighlightedToken(tok))
                        return 1;
                    break;
                }
                case FL_LEAVE: {
                    if (ChangeHighlightedToken(nullptr))
                        return 1;
                    break;
                }
                case FL_PUSH: {
                    if (Fl::event_button()==FL_LEFT_MOUSE && tok) {
                        //std::cout << "FL_PUSH" << std::endl;
                        pDragOffsetX = Fl::event_x() - tok->x();
                        pDragOffsetY = Fl::event_y() - tok->y();
                        pDraggedToken = tok;
                        insert(*tok, children());
                        parent()->redraw();
                        return 1;
                    }
                    break;
                }
                case FL_DRAG: {
                    if (pDraggedToken) {
                        int borderWidthX = Fl::box_dx(box());
                        int borderWidthY = Fl::box_dy(box());
                        int newX = Fl::event_x() - pDragOffsetX;
                        int newY = Fl::event_y() - pDragOffsetY;
                        
                        Clip(newX,
                                x()+borderWidthX,
                                x()+w()-borderWidthX-tok->w());
                        Clip(newY,
                                y()+borderWidthY,
                                y()+h()-borderWidthY-tok->h());

                        auto[tokenX,tokenY] = pDraggedToken->Center();
                        Cell* closestCell = Snap(tokenX, tokenY);
                        if (!closestCell || closestCell->Empty() || closestCell->Content() == pDraggedToken)
                            ChangeHighlightedCell(closestCell);

                        pDraggedToken->position(newX, newY);
                        parent()->redraw();
                        return 1;
                    }
                    break;
                }
                case FL_RELEASE: {
                    //std::cout << "FL_RELEASE" << std::endl;
                    if (pDraggedToken) {
                        auto[tokenX,tokenY] = pDraggedToken->Center();

                        Cell* closestCell = Snap(tokenX, tokenY);
                        if (closestCell && !closestCell->Empty() && closestCell->Content()!=pDraggedToken)
                            closestCell = nullptr;
                        else if (!closestCell && IsWithin(tokenX, tokenY, pBoard))
                            closestCell = pDraggedToken->At();

                        if (closestCell)
                            MoveToken(pDraggedToken, closestCell);
                        else {
                            RemoveToken(pDraggedToken);
                            Fl::belowmouse(this);
                        }

                        init_sizes();
                        ChangeHighlightedCell(nullptr);
                        pDraggedToken = nullptr;
                        parent()->redraw();
                        return 1;
                    }
                    break;
                }
                default:
                    break;
            }
            return Fl_Group::handle(event);
        }

        void MoveToken(Token* tok, Cell* newLocation) {
            if (tok->At())
                tok->At()->Content(nullptr);
            tok->At(newLocation);
            newLocation->Content(tok);
            tok->resize(newLocation->x(), newLocation->y(), newLocation->w(), newLocation->h());
        }

        void RemoveToken(Token* tok) {
            if (tok->At())
                tok->At()->Content(nullptr);
            tok->At(nullptr);
            pOutOfBoardView->insert(*tok, pOutOfBoardView->children());
        }

        bool ChangeHighlightedCell(Cell* newCell) {
            if (newCell != pHighlightedCell) {
                if (pHighlightedCell)
                    pHighlightedCell->ResetColor();
                if (newCell)
                    newCell->Highlight();
                pHighlightedCell = newCell;
                redraw();
                return true;
            }
            return false;
        }

        bool ChangeHighlightedToken(Token* newToken) {
            if (newToken != pHighlightedToken) {
                if (pHighlightedToken)
                    pHighlightedToken->ResetColor();
                if (newToken)
                    newToken->Highlight();
                pHighlightedToken = newToken;
                redraw();
                return true;
            }
            return false;
        }

        Cell* Snap(int X, int Y) {
            Cell* cell = nullptr;
            int s = TileSize();
            int g = CellGap();
            //int i = (Y - pBoard->y() - g)/(s+g);
            //int j = (X - pBoard->x() - g)/(s+g);
            int i = (Y - pBoard->y() + s)/(s+g) - 1;
            int j = (X - pBoard->x() + s)/(s+g) - 1;
            if (IsBetween(i, 0, kBoardHeightTiles-1) && IsBetween(j, 0, kBoardWidthTiles-1)) {
                cell = At(i, j);
                if (!IsWithin(X, Y, cell))
                    cell = nullptr;
            }
            return cell;
        }

        Cell* At(int i, int j) {
            return pCells[i*kBoardWidthTiles + j];
        }

        int TileSize() const {
            return pCells[0]->w();
        }

        int CellGap() const {
            return pCells[0]->x() - pBoard->x();
        }

        void InitBoard() {
            int boardWidth = (kCellMargin+kTileSize)*kBoardWidthTiles + kCellMargin;
            int boardHeight = (kCellMargin+kTileSize)*kBoardHeightTiles + kCellMargin;
            auto[boardX,boardY] = AlignCenter(w(), h(), boardWidth, boardHeight);
            pBoard = new Fl_Box(boardX, boardY, boardWidth, boardHeight);
            pBoard->box(FL_FLAT_BOX);
            pBoard->color(FL_BLACK);
            for (size_t i = 0; i < kBoardHeightTiles; ++i) {
                for (size_t j = 0; j < kBoardWidthTiles; ++j) {
                    int cellX = boardX + kCellMargin + j*(kTileSize+kCellMargin);
                    int cellY = boardY + kCellMargin + i*(kTileSize+kCellMargin);
                    auto type = i == 0? Cell::GOAL : Cell::NORMAL;
                    pCells[i*kBoardWidthTiles+j] = new Cell(cellX, cellY, type);
                }
            }

            // Y axis labels
            for (size_t i = 0; i < kBoardHeightTiles; ++i) {
                int labelX = pBoard->x() - kTileSize - kCellMargin + kTileSize/4;
                int labelY = pBoard->y() + kCellMargin + i*(kTileSize+kCellMargin) + kTileSize/4;
                char label[] = {0, 0};
                label[0] = '0' + (kBoardHeightTiles - i);
                auto* labelBox = new Fl_Box(labelX, labelY, kTileSize/2, kTileSize/2);
                labelBox->copy_label(label);
                labelBox->color(FL_WHITE);
                labelBox->box(FL_BORDER_BOX);
            }

            // X axis labels
            for (size_t j = 0; j < kBoardWidthTiles; ++j) {
                int labelX = pBoard->x() + kCellMargin + j*(kTileSize+kCellMargin) + kTileSize/4;
                int labelY = pBoard->y() + pBoard->h() + kCellMargin + kTileSize/4;
                char label[] = {0, 0};
                label[0] = 'A' + j;
                auto* labelBox = new Fl_Box(labelX, labelY, kTileSize/2, kTileSize/2);
                labelBox->copy_label(label);
                labelBox->color(FL_WHITE);
                labelBox->box(FL_BORDER_BOX);
            }
        }

        void InitOutOfBoardView() {
            const int margin = 20;
            int outOfBoardX = x()+margin;
            int outOfBoardY = y()+margin;
            int outOfBoardW = w() - 2*margin;
            pOutOfBoardView = new Fl_Pack(outOfBoardX, outOfBoardY, outOfBoardW, kTileSize);
            pOutOfBoardView->type(Fl_Pack::HORIZONTAL);
            pOutOfBoardView->spacing(kCellMargin);
            pOutOfBoardView->resizable(pOutOfBoardView);
            pOutOfBoardView->end();
        }

        Fl_Box* pBoard;
        std::vector<Cell*> pCells;
        Token* pHighlightedToken;
        Token* pDraggedToken;
        Cell* pHighlightedCell;
        Fl_Pack* pOutOfBoardView;
        int pDragOffsetX;
        int pDragOffsetY;
        bool pEditable;

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

} // anonymous ns

class GameGUI::Impl {
    public:
        Impl(bool editable) : pEditable(editable) {
            pWin = std::make_unique<Fl_Double_Window>(kDefaultWindowWidth, kDefaultWindowHeight, "Board");
            InitSplitWindow();
            pWin->end();
        }

        void ResetBoardStatus(const TokenArray& tokens) {
            FlLock lock;
            pMainView->ResetBoardStatus(tokens);
        }

        void ResetTokensOutOfBoard() {
            FlLock lock;
            pMainView->ResetTokensOutOfBoard();
        }

        void ResetTokensOutOfBoard(const TokenArray& tokens) {
            FlLock lock;
            pMainView->ResetTokensOutOfBoard(tokens);
        }

        void Log(const std::string& message, LogLevel LogLevel) {
            FlLock lock;
            std::string dateAndTime = DateAndTime();
            const char* colorCode = kColorCodes[static_cast<int>(LogLevel)];
            pLogTerminal->printf("%s: %s%s\033[0m\n", dateAndTime.c_str(), colorCode, message.c_str());
        }

        TokenArray GetBoardStatus() const {
            FlLock lock;
            return pMainView->GetBoardStatus();
        }

        TokenArray GetTokensOutOfBoard() const {
            FlLock lock;
            return pMainView->GetTokensOutOfBoard();
        }

        void Close() {
            Fl::awake(&Impl::Close, this);
        }

        int Run(int argc, char* argv[]) {
            Fl::lock();
            pWin->resizable(pWin.get());
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
            int topTileHeight = p->h() - kDefaultTerminalHeight;
            auto* topTile = new Fl_Group(p->x(), p->y(), p->w(), topTileHeight);
            topTile->box(FL_DOWN_BOX);
            topTile->clip_children(1);
            pMainView = new GameArea(pEditable);
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
        GameArea* pMainView;
        Fl_Simple_Terminal* pLogTerminal;
        bool pEditable;
        
};


GameGUI::GameGUI(bool editable) : pImpl(new Impl(editable)) {

}

void GameGUI::ResetBoardStatus(const TokenArray& tokens) {
    pImpl->ResetBoardStatus(tokens);
}

void GameGUI::ResetTokensOutOfBoard() {
    pImpl->ResetTokensOutOfBoard();
}

void GameGUI::ResetTokensOutOfBoard(const TokenArray& tokens) {
    pImpl->ResetTokensOutOfBoard(tokens);
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

