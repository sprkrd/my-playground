#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/fl_draw.H>


typedef std::unique_ptr<Fl_Window> Fl_WindowPtr;
typedef std::unique_ptr<Fl_Box> Fl_BoxPtr;

Fl_Color NORMAL_COLOR_BG = fl_rgb_color(  0, 100, 210);
Fl_Color   GOAL_COLOR_BG = fl_rgb_color(200, 200, 200);
Fl_Color  TABLE_COLOR_BG = fl_rgb_color(150, 105,  46);
Fl_Color  TOKEN_COLOR_BG = fl_rgb_color(220,  75,  25);
Fl_Color  BOARD_COLOR_BG = FL_BLACK;

Fl_Color  TOKEN_COLOR_FG = FL_WHITE;

enum class CellType { NORMAL = 0, GOAL, TABLE };
Fl_Color CELL_BG_COLORS[] = {NORMAL_COLOR_BG, GOAL_COLOR_BG, TABLE_COLOR_BG};


class Cell : public Fl_Button {
    public:

        typedef std::unique_ptr<Cell> Ptr;

        Cell(int x, int y, int size, CellType type = CellType::NORMAL)
            : Fl_Button(x, y, size, size), m_type(type) {
            reset_color();
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

        void reset_content(const std::string& content, bool copy = true) {
            if (copy && !content.empty())
                copy_label(content.c_str());
            else
                label(content.empty()? nullptr : content.c_str());
            reset_color();
        }

        void draw() {
            if (m_type == CellType::TABLE && empty()) {
                fl_line_style(FL_DOT);
                fl_rect(x(), y(), w(), h(), FL_BLACK);
                fl_line_style(0);
            }
            else {
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

constexpr int DEFAULT_WINDOW_WIDTH = 800;
constexpr int DEFAULT_WINDOW_HEIGHT = 640;
constexpr int DEFAULT_BOARD_WIDTH_TILES = 5;
constexpr int DEFAULT_BOARD_HEIGHT_TILES = 4;
constexpr int DEFAULT_TILE_SIZE = 50;
constexpr int DEFAULT_WINDOW_PADDING = 50;
constexpr int DEFAULT_TABLE_PADDING = 20;
constexpr int DEFAULT_CELL_MARGIN = 20;;

class GameEnv {
    public:

        struct Config {
            int window_width = DEFAULT_WINDOW_WIDTH;
            int window_height = DEFAULT_WINDOW_HEIGHT;
            int board_width_tiles = DEFAULT_BOARD_WIDTH_TILES;
            int board_height_tiles = DEFAULT_BOARD_HEIGHT_TILES;
            int tile_size = DEFAULT_TILE_SIZE;
            int window_padding = DEFAULT_WINDOW_PADDING;
            int table_padding = DEFAULT_TABLE_PADDING;
            int cell_margin = DEFAULT_CELL_MARGIN;
        };

        GameEnv() : m_config() {
            init();
        }

        const Config& config() const {
            return m_config;
        }

        void config(const Config& config) {
            m_config = config;
        }

        void show() {
            m_win->show();
        }

        void show(int argc, char* argv[]) {
            m_win->show(argc, argv);
        }


   private:

        void init() {
            const Config& cfg = m_config;
            int number_of_tiles = cfg.board_width_tiles*cfg.board_height_tiles;
            int table_width = cfg.window_width - 2*cfg.window_padding;
            int table_height = cfg.window_height - 2*cfg.window_padding;
            int x_table = cfg.window_padding;
            int y_table = cfg.window_padding;
            int board_width = cfg.board_width_tiles*(cfg.tile_size+cfg.cell_margin) + cfg.cell_margin;
            int board_height = cfg.board_height_tiles*(cfg.tile_size+cfg.cell_margin) + cfg.cell_margin;
            int x_board = x_table + (table_width - board_width)/2;
            int y_board = y_table + (table_height - board_height)/2;

            m_win = std::make_unique<Fl_Double_Window>(m_config.window_width,
                                                       m_config.window_height);
            //m_win->color(FL_WHITE);
            m_table = std::make_unique<Fl_Box>(x_table, y_table, table_width, table_height);
            m_table->box(FL_SHADOW_BOX);
            m_table->color(TABLE_COLOR_BG);
            m_board = std::make_unique<Fl_Box>(x_board, y_board, board_width, board_height);
            m_board->box(FL_FLAT_BOX);
            m_board->color(BOARD_COLOR_BG);

            m_cells.resize(number_of_tiles);
            for (int i = 0; i < cfg.board_height_tiles; ++i) {
                for (int j = 0; j < cfg.board_width_tiles; ++j) {
                    int idx = i*cfg.board_width_tiles + j;
                    int x_cell = x_board + cfg.cell_margin + j*(cfg.cell_margin+cfg.tile_size);
                    int y_cell = y_board + cfg.cell_margin + i*(cfg.cell_margin+cfg.tile_size);
                    CellType cell_type = i==0? CellType::GOAL : CellType::NORMAL;
                    m_cells[idx] = std::make_unique<Cell>(x_cell, y_cell, cfg.tile_size, cell_type);
                }
            }

            m_row_labels.resize(cfg.board_height_tiles);
            for (int i = 0; i < cfg.board_height_tiles; ++i) {
                int x_label = x_board - cfg.tile_size;
                int y_label = y_board + cfg.cell_margin + i*(cfg.cell_margin+cfg.tile_size);
                char row_id = '1' + (cfg.board_height_tiles - i - 1);
                m_row_labels[i] = create_label_box(x_label, y_label, std::string(1, row_id));
            }

            m_col_labels.resize(cfg.board_width_tiles);
            for (int j = 0; j < cfg.board_width_tiles; ++j) {
                int x_label = x_board + cfg.cell_margin + j*(cfg.cell_margin+cfg.tile_size);
                int y_label = y_board + board_height;
                char col_id = 'A' + j;
                m_col_labels[j] = create_label_box(x_label, y_label, std::string(1, col_id));
            }
        }

        Fl_BoxPtr create_label_box(int x, int y, const std::string& label) {
            int size = m_config.tile_size/2;
            x += (m_config.tile_size - size)/2;
            y += (m_config.tile_size - size)/2;
            auto box = std::make_unique<Fl_Box>(x, y, m_config.tile_size/2, m_config.tile_size/2);
            box->copy_label(label.c_str());
            box->box(FL_BORDER_BOX);
            box->color(FL_WHITE);
            return box;
        }

        Config m_config;
        Fl_WindowPtr m_win;
        Fl_BoxPtr m_table, m_board;
        std::vector<Fl_BoxPtr> m_row_labels, m_col_labels;
        std::vector<Cell::Ptr> m_cells;
};




//void button_callback(Fl_Widget* widget, void* data) {
    //auto button = static_cast<Cell*>(widget);
    //auto board = static_cast<Cell**>(data);
    //button->highlighted(true);
    //for (int h = 0; h < k_width_tiles; ++h) {
        //for (int v = 0; v < k_height_tiles; ++v) {
            //auto other = board[v*k_width_tiles + h];
            //if (other == button)
                //continue;
            //other->deactivate();
        //}
    //}
    ////button->parent()->redraw();
//}


int main(int argc, char **argv) {

    GameEnv env;
    env.show(argc, argv);
    return Fl::run();

    //auto window = new Fl_Double_Window(k_window_size, k_window_size, "board");
    //auto box = new Fl_Box(k_window_padding, k_window_padding,
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
