#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>

#include <tuple>

typedef std::tuple<int,int> Point2i;


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


int main(int argc, char **argv) {
  Fl_Window* window = new Fl_Window(512, 512);
  Fl_Group* myGroup = new CenteredGroup(512, 512);

  new Fl_Button(128, 128, 100, 50, "Hey!");

  myGroup->box(FL_FLAT_BOX);
  myGroup->color(FL_BLUE);
  myGroup->end();
  window->end();
  window->resizable(window);
  window->show(argc, argv);
  return Fl::run();
}
