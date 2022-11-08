#include <array>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Scroll.H>
#include <FL/fl_draw.H>

typedef std::pair<int,int> Point2i;

constexpr int DEFAULT_BOARD_WIDTH_TILES = 5;
constexpr int DEFAULT_BOARD_HEIGHT_TILES = 4;
constexpr int DEFAULT_TILE_SIZE = 50;
constexpr int DEFAULT_MAIN_VIEW_PADDING = 20;
constexpr int DEFAULT_TABLE_PADDING = 10;
constexpr int DEFAULT_CELL_MARGIN = 20;

constexpr int BORDER_OUT_HEIGHT = 30;

constexpr int WIN_WIDTH = 800;
constexpr int WIN_HEIGHT = 640;


const Fl_Color NORMAL_COLOR_BG = fl_rgb_color(  0, 100, 210);
const Fl_Color   GOAL_COLOR_BG = fl_rgb_color(200, 200, 200);
const Fl_Color  TABLE_COLOR_BG = fl_rgb_color(150, 105,  46);
const Fl_Color  TOKEN_COLOR_BG = fl_rgb_color(220,  75,  25);
const Fl_Color  BOARD_COLOR_BG = FL_BLACK;
const Fl_Color   BORDER_OUT_BG = NORMAL_COLOR_BG;
const Fl_Color          WIN_BG = FL_WHITE;
const Fl_Color  TOKEN_COLOR_FG = FL_WHITE;
const Fl_Color        LABEL_BG = FL_WHITE;

enum class CellType { NORMAL = 0, GOAL, TABLE};
Fl_Color CELL_BG_COLORS[] = {NORMAL_COLOR_BG, GOAL_COLOR_BG, TABLE_COLOR_BG};


class Cell : public Fl_Button {
    public:
        Cell(int x, int y, int size, CellType type = CellType::NORMAL)
            : Fl_Button(x, y, size, size), m_type(type) {
            reset_content();
        }

        bool empty() const {
            return label() == nullptr;
        }

        bool highlighted() const {
            return m_highlighted;
        }

        void highlighted(bool new_value) {
            if (new_value != m_highlighted) {
                m_highlighted = new_value;
                redraw();
                redraw_label();
            }
        }

        bool out_of_board() const {
            return m_type == CellType::TABLE;
        }

        void reset_content(const char* content = nullptr) {
            copy_label(content);
            if (content)
                activate();
            else
                deactivate();
            highlighted(false);
            reset_color();
        }

        void draw() {
            Fl_Color bg = get_bg_color();
            fl_color(bg);
            fl_pie(x(), y(), w(), h(), 0, 360);
            if (!empty()) {
                Fl_Color fg = fl_contrast(TOKEN_COLOR_FG, bg);
                fl_color(fg);
                fl_font(labelfont(), labelsize());
                fl_draw(label(), x(), y(), w(), h(), align(), nullptr, 1);
            }
        }

    private:

        Fl_Color get_bg_color() const {
            Fl_Color bg = color();
            if (value() || m_highlighted)
                bg = fl_color_average(bg, FL_WHITE, 0.75);
            else if (!active())
                bg = fl_color_average(bg, FL_BLACK, 0.75);
            return bg;
        }

        void reset_color() {
            if (empty()) {
                int bg_color_index = static_cast<int>(m_type);
                color(CELL_BG_COLORS[bg_color_index]);
            }
            else
                color(TOKEN_COLOR_BG);
        }

        CellType m_type;
        bool m_highlighted;
};

class PushableScroll : public Fl_Scroll {
    public:
        PushableScroll(int x, int y, int w, int h) : Fl_Scroll(x, y, w, h),
                                                     m_pushable(false) {}

        virtual int handle(int e) override {
            if (m_pushable && e == FL_PUSH) {
                do_callback();
                return 1;
            }
            return Fl_Scroll::handle(e);
        }

        bool pushable() const {
            return m_pushable;
        }

        void pushable(bool value) {
            m_pushable = value;
        }

    private:

        bool m_pushable;

};

struct GameEnvConfig {
    int board_width_tiles = DEFAULT_BOARD_WIDTH_TILES;
    int board_height_tiles = DEFAULT_BOARD_HEIGHT_TILES;
    int tile_size = DEFAULT_TILE_SIZE;
    int game_view_padding = DEFAULT_MAIN_VIEW_PADDING;
    int table_padding = DEFAULT_TABLE_PADDING;
    int cell_margin = DEFAULT_CELL_MARGIN;
    bool editable = true;
};

class GameEnv {
    public:
        GameEnv(const GameEnv& other) = delete;

        GameEnv(int x = 0, int y = 0, int w = -1, int h = -1,
                GameEnvConfig cfg = GameEnvConfig()) : m_config(cfg),
                                                       m_selected(nullptr) {
            if (w == -1)
                w = Fl_Group::current()->w() - x;
            if (h == -1)
                h = Fl_Group::current()->h() - y;
            m_view = new Fl_Group(x, y, w, h);
            init();
            m_view->end();
        }

        GameEnv& operator=(const GameEnv& other) = delete;

        std::string token_at(const std::string& position) const {
            const Cell* cell = cell_at(position);
            if (cell->empty())
                return std::string();
            return std::string(cell->label());
        }

        void schedule_for_deletion() {
            Fl::delete_widget(m_view);
            m_view = nullptr;
        }

        void remove_token_at(const std::string& position) {
            Cell* cell = cell_at(position);
            cell->reset_content();
        }

        void add_token_at(const std::string& token, const std::string& position) {
            Cell* cell = cell_at(position);
            cell->reset_content(token.c_str());
        }

        void add_token(const std::string& token) {
            int y = m_out_view->y() + (m_out_view->h() - m_config.tile_size)/2;
            int x;
            if (m_out_of_board.empty())
                x = m_out_view->x() + m_config.cell_margin;
            else
                x = m_out_of_board.back()->x() + m_config.cell_margin + m_config.tile_size;
            auto current = Fl_Group::current();
            Fl_Group::current(m_out_view);
            auto cell = new Cell(x, y, m_config.tile_size, CellType::TABLE);
            cell->reset_content(token.c_str());
            if (m_config.editable)
                cell->callback(GameEnv::out_of_board_cell_static_cb, this);
            m_out_of_board.push_back(cell);
            Fl_Group::current(current);
            m_out_view->redraw();
        }

        void reset() {
            m_selected = nullptr;
            for (auto* cell : m_cells)
                cell->reset_content();
            clear_out_of_table();
        }

        void reset(const std::vector<std::string>& board) {
            m_selected = nullptr;
            if (board.size() != m_cells.size())
                throw std::invalid_argument("Invalid vector size size");
            for (size_t i = 0; i < board.size(); ++i)
                m_cells[i]->reset_content(board[i].empty()? nullptr : board[i].c_str());
            clear_out_of_table();
        }

        void clear_out_of_table() {
            for (auto* cell : m_out_of_board)
                Fl::delete_widget(cell);
            m_out_of_board.clear();
            m_out_view->redraw();
        }

        std::vector<std::string> status() const {
            std::vector<std::string> result;
            result.reserve(m_cells.size());
            for (const Cell* cell : m_cells) {
                if (cell->empty())
                    result.emplace_back();
                else
                    result.emplace_back(cell->label());
            }
            return result;
        }

        ~GameEnv() {
            delete m_view;
        }

    private:

        Cell* cell_at(const std::string& position) {
            const GameEnv* const_this = this;
            return const_cast<Cell*>(cell_at(position));
        }

        const Cell* cell_at(const std::string& position) const {
            int idx = to_index(id_to_point2i(position));
            return m_cells[idx];
        }

        int to_index(const Point2i& point) const {
            auto[i,j] = point;
            return i*m_config.board_width_tiles + j;
        }

        bool is_valid_position_id(const std::string& id) const {
            return id.length()==2 &&
                   'A'<=id[0] && id[0]<'A'+m_config.board_width_tiles &&
                   '1'<=id[1] && id[1]<'1'+m_config.board_height_tiles;
        }

        Point2i id_to_point2i(const std::string& id) const {
            Point2i result;
            if (!is_valid_position_id(id))
                throw std::invalid_argument("Position " + id + " is not a valid");
            result.first = m_config.board_height_tiles - (id[1]-'1'+1);
            result.second = id[0] - 'A';
            return result;
        }

        void init() {
            const auto& cfg = m_config;

            // create table
            int table_width = m_view->w() - 2*cfg.game_view_padding;
            int table_height = m_view->h() - 2*cfg.game_view_padding;
            int x_table = m_view->x() + cfg.game_view_padding;
            int y_table = m_view->y() + cfg.game_view_padding;
            auto table = new Fl_Box(x_table, y_table, table_width, table_height);
            table->box(FL_SHADOW_BOX);
            table->color(TABLE_COLOR_BG);

            // create out of board subgroup
            int y_border_out = y_table + cfg.table_padding + 1;
            int x_out = x_table + cfg.table_padding;
            int y_out = y_border_out + BORDER_OUT_HEIGHT - 1;
            int width_out = table_width - 2*cfg.table_padding;
            int height_out = cfg.tile_size*2;
            auto border_out = new Fl_Box(x_out, y_table+cfg.table_padding+1, width_out, BORDER_OUT_HEIGHT);
            border_out->box(FL_BORDER_BOX);
            border_out->color(BORDER_OUT_BG);
            border_out->labelcolor(fl_contrast(FL_BLACK, BORDER_OUT_BG));
            border_out->label("Tokens outside of board");
            m_out_view = new PushableScroll(x_out, y_out, width_out, height_out);
            m_out_view->type(Fl_Scroll::HORIZONTAL);
            m_out_view->color(WIN_BG);
            m_out_view->box(FL_BORDER_BOX);
            if (cfg.editable)
                m_out_view->callback(GameEnv::out_of_board_static_cb, this);
            m_out_view->end();

            // create board
            int board_width = cfg.board_width_tiles*(cfg.tile_size+cfg.cell_margin) + cfg.cell_margin;
            int board_height = cfg.board_height_tiles*(cfg.tile_size+cfg.cell_margin) + cfg.cell_margin;
            int x_board = x_table + (table_width - board_width)/2;
            int y_board = y_table + (table_height - board_height)/2;
            auto board = new Fl_Box(x_board, y_board, board_width, board_height);
            board->box(FL_FLAT_BOX);
            board->color(BOARD_COLOR_BG);

            // create board cells
            int number_of_tiles = cfg.board_width_tiles*cfg.board_height_tiles;
            m_cells.resize(number_of_tiles);
            for (int i = 0; i < cfg.board_height_tiles; ++i) {
                for (int j = 0; j < cfg.board_width_tiles; ++j) {
                    int idx = i*cfg.board_width_tiles + j;
                    int x_cell = x_board + cfg.cell_margin + j*(cfg.cell_margin+cfg.tile_size);
                    int y_cell = y_board + cfg.cell_margin + i*(cfg.cell_margin+cfg.tile_size);
                    CellType cell_type = i==0? CellType::GOAL : CellType::NORMAL;
                    m_cells[idx] = new Cell(x_cell, y_cell, cfg.tile_size, cell_type);
                    if (cfg.editable)
                        m_cells[idx]->callback(GameEnv::board_cell_static_cb, this);
                }
            }

            // create vertical axis labels
            for (int i = 0; i < cfg.board_height_tiles; ++i) {
                int x_label = x_board - cfg.tile_size;
                int y_label = y_board + cfg.cell_margin + i*(cfg.cell_margin+cfg.tile_size);
                char row_id = '1' + (cfg.board_height_tiles - i - 1);
                create_label_box(x_label, y_label, std::string(1, row_id));
            }

            // create horizontal axis labels
            for (int j = 0; j < cfg.board_width_tiles; ++j) {
                int x_label = x_board + cfg.cell_margin + j*(cfg.cell_margin+cfg.tile_size);
                int y_label = y_board + board_height;
                char col_id = 'A' + j;
                create_label_box(x_label, y_label, std::string(1, col_id));
            }
        }

        Fl_Box* create_label_box(int x, int y, const std::string& label) {
            int size = m_config.tile_size/2;
            x += (m_config.tile_size - size)/2;
            y += (m_config.tile_size - size)/2;
            auto box = new Fl_Box(x, y, m_config.tile_size/2, m_config.tile_size/2);
            box->copy_label(label.c_str());
            box->box(FL_BORDER_BOX);
            box->color(LABEL_BG);
            return box;
        }

        void move(Cell* src, Cell* dst) {
            dst->reset_content(src->label());
            src->reset_content();
        }

        void remove_token_from_out_of_board(Cell* src) {
            bool found = false;
            int x = src->x();
            for (auto it = m_out_of_board.begin(); it != m_out_of_board.end();) {
                if (found) {
                    (*it)->position(x,src->y());
                    x += m_config.cell_margin + m_config.tile_size;
                    ++it;
                }
                else if (*it == src) {
                    found = true;
                    it = m_out_of_board.erase(it);
                }
                else
                    ++it;
            }
            Fl::delete_widget(src);
        }

        void move_to_board(Cell* src, Cell* dst) {
            dst->reset_content(src->label());
            remove_token_from_out_of_board(src);
        }

        void move_out_of_board(Cell* src) {
            add_token(src->label());
            src->reset_content();
        }

        std::vector<Cell*> get_all_cells() const {
            auto result = m_cells;
            result.insert(result.end(), m_out_of_board.begin(), m_out_of_board.end());
            return result;
        }

        void select(Cell* cell) {
            m_selected = cell;
            cell->highlighted(true);
            std::vector<Cell*> all_cells = get_all_cells();
            for (Cell* other : all_cells) {
                if (other->empty())
                    other->activate();
                else if (cell != other)
                    other->deactivate();
            }
        }

        void unselect() {
            m_selected->highlighted(false);
            m_selected = nullptr;
            std::vector<Cell*> all_cells = get_all_cells();
            for (Cell* other : all_cells) {
                if (other->empty())
                    other->deactivate();
                else
                    other->activate();
            }
        }

        void board_cell_cb(Cell* dst) {
            if (m_selected == dst) {
                unselect();
                m_out_view->pushable(false);
            }
            else if (m_selected) {
                Cell* cpy_selected = m_selected;
                unselect();
                if (cpy_selected->out_of_board())
                    move_to_board(cpy_selected, dst);
                else
                    move(cpy_selected, dst);
                m_out_view->pushable(false);
            }
            else {
                select(dst);
                m_out_view->pushable(true);
            }
        }

        void out_of_board_cell_cb(Cell* dst) {
            Cell* cpy_selected = m_selected;
            if (m_selected)
                unselect();
            if (cpy_selected != dst)
                select(dst);
        }

        void out_of_board_cb() {
            if (m_selected && !m_selected->out_of_board()) {
                move_out_of_board(m_selected);
                unselect();
                m_out_view->pushable(false);
            }
        }

        static void board_cell_static_cb(Fl_Widget* w, void* data) {
            auto cell = static_cast<Cell*>(w);
            auto game_env = static_cast<GameEnv*>(data);
            game_env->board_cell_cb(cell);
        }

        static void out_of_board_cell_static_cb(Fl_Widget* w, void* data) {
            auto cell = static_cast<Cell*>(w);
            auto game_env = static_cast<GameEnv*>(data);
            game_env->out_of_board_cell_cb(cell);
        }

        static void out_of_board_static_cb(Fl_Widget*, void* data) {
            auto game_env = static_cast<GameEnv*>(data);
            game_env->out_of_board_cb();
        }

        GameEnvConfig m_config;
        Fl_Group* m_view;
        PushableScroll* m_out_view;
        std::list<Cell*> m_out_of_board;
        std::vector<Cell*> m_cells;
        Cell* m_selected;
};


int main(int argc, char **argv) {

    auto win = new Fl_Double_Window(WIN_WIDTH, WIN_HEIGHT);
    GameEnv env;
    for (int i = 0; i < 26; ++i) {
        env.add_token(std::string(1,'A'+i));
    }
    win->show(argc, argv);
    win->color(WIN_BG);
    return Fl::run();

    //auto window = new Fl_Double_Window(k_window_size, k_window_size, "board");
    //auto box = new Fl_Box(k_game_view_padding, k_game_view_padding,
                          //k_main_area_size, k_main_area_size, "lerelelele");
    //box->box(FL_UP_BOX);

    //Cell* board[k_height_tiles][k_width_tiles];

    //for (int h = 0; h < k_width_tiles; ++h) {
        //for (int v = 0; v < k_height_tiles; ++v) {
            //CellType cell_type = v==k_height_tiles-1? CellType::GOAL : CellType::NORMAL;
            //int x = a1_bottom_x + h*(k_tile_size+k_board_gap);
            //int y = a1_bottom_y - v*(k_tile_size+k_board_gap);
            //char label[3] = {0};
            //label[0] = 'A' + h;
            //label[1] = '1' + v;
            //board[v][h] = new Cell(x, y, k_tile_size, cell_type);
            //board[v][h]->callback(button_callback, &board);
            //board[v][h]->reset_content(label);
        //}
    //}

    //auto cell = new Fl_Button(50, 50, k_tile_size, k_tile_size, "hey");
    ////box->labelfont(FL_BOLD+FL_ITALIC);
    ////box->labelsize(36);
    ////box->labeltype(FL_SHADOW_LABEL);
    //window->end();
    //window->show(argc, argv);
    //return Fl::run();
}
