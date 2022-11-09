#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Scroll.H>
 
int main(int argc, char **argv) {
  Fl_Window *window = new Fl_Window(340,180);

  Fl_Scroll *scroll = new Fl_Scroll(0, 70, 100, 100);

  Fl_Pack *pack = new Fl_Pack(0, 70, 10, 40);
  pack->spacing(20);
  pack->type(Fl_Pack::HORIZONTAL);

  Fl_Box* box1 = new Fl_Box(0, 0, 50, 1);
  box1->box(FL_FLAT_BOX);
  box1->color(FL_RED);

  Fl_Box* box2 = new Fl_Box(0, 0, 50, 1);
  box2->box(FL_FLAT_BOX);
  box2->color(FL_BLUE);

  Fl_Box* box3 = new Fl_Box(0, 0, 50, 1);
  box3->box(FL_FLAT_BOX);
  box3->color(FL_RED);

  pack->end();

  scroll->end();

  window->end();
  window->show(argc, argv);
  return Fl::run();
}
