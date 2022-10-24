#include <bitset>
#include <iostream>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/fl_draw.H>

constexpr int k_window_size = 500;
constexpr int k_window_padding = 20;
constexpr int k_width_tiles = 5;
constexpr int k_height_tiles = 4;
constexpr int k_tile_size = 50;

constexpr int k_main_area_size = k_window_size - 2*k_window_padding;
constexpr int k_board_h_margin = (k_main_area_size - k_tile_size*k_width_tiles)/2;
constexpr int k_board_v_margin = (k_main_area_size - k_tile_size*k_height_tiles)/2;

constexpr int a1_bottom_x = k_window_padding + k_board_h_margin;
constexpr int a1_bottom_y = k_window_padding + k_main_area_size - k_board_v_margin - k_tile_size;

Fl_Color NORMAL_COLOR_BG = fl_rgb_color(  0, 100, 210);
Fl_Color   GOAL_COLOR_BG = fl_rgb_color(220, 220, 220);
Fl_Color  TABLE_COLOR_BG = fl_rgb_color(150, 105,  46);
Fl_Color  TOKEN_COLOR_BG = fl_rgb_color(220,  75,  25);

Fl_Color  TOKEN_COLOR_FG = fl_rgb_color(255, 255, 255);

Fl_Color BG_COLORS[] = {NORMAL_COLOR_BG, GOAL_COLOR_BG, TABLE_COLOR_BG};

enum class CellType { NORMAL = 0, GOAL, TABLE };

class Cell : public Fl_Button {
    public:

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
            m_highlighted = new_value;
        }

        void reset_content(const char* content = nullptr) {
            label(content);
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
                color(BG_COLORS[bg_color_index]);
            }
            else
                color(TOKEN_COLOR_BG);
        }

        CellType m_type;
        bool m_highlighted;
};

void button_callback(Fl_Widget* widget, void* data) {
    auto button = static_cast<Cell*>(widget);
    auto board = static_cast<Cell**>(data);
    std::cout << button->label() << std::endl;
    button->highlighted(true);
    for (int h = 0; h < k_width_tiles; ++h) {
        for (int v = 0; v < k_height_tiles; ++v) {
            auto other = board[v*k_width_tiles + h];
            if (other == button)
                continue;
            other->deactivate();
        }
    }
    //button->parent()->redraw();
}


int main(int argc, char **argv) {
    auto window = new Fl_Double_Window(k_window_size, k_window_size, "board");
    auto box = new Fl_Box(k_window_padding, k_window_padding,
                          k_main_area_size, k_main_area_size, nullptr);
    box->box(FL_UP_BOX);

    Cell* board[k_height_tiles][k_width_tiles];

    for (int h = 0; h < k_width_tiles; ++h) {
        for (int v = 0; v < k_height_tiles; ++v) {
            int x = a1_bottom_x + h*k_tile_size;
            int y = a1_bottom_y - v*k_tile_size;
            char label[3] = {0};
            label[0] = 'A' + h;
            label[1] = '1' + v;
            board[v][h] = new Cell(x, y, k_tile_size);
            board[v][h]->callback(button_callback, &board);
            board[v][h]->copy_label(label);
        }
    }

    auto cell = new Fl_Button(50, 50, k_tile_size, k_tile_size, "hey");
    //box->labelfont(FL_BOLD+FL_ITALIC);
    //box->labelsize(36);
    //box->labeltype(FL_SHADOW_LABEL);
    window->end();
    window->show(argc, argv);
    return Fl::run();
}
