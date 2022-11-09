#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
 
int main(int argc, char **argv) {
  Fl_Window *window = new Fl_Window(512, 512);
  Fl_Box* spacer1 = new Fl_Box(0, 0, 128, 128, nullptr);
  spacer1->box(FL_FLAT_BOX);
  spacer1->color(FL_BLUE);
  Fl_Box* spacer2 = new Fl_Box(384, 384, 128, 128, nullptr);
  spacer2->box(FL_FLAT_BOX);
  spacer2->color(FL_RED);
  Fl_Group* mainViewport = new Fl_Group(128, 128, 256, 256, nullptr);
  mainViewport->resizable(nullptr);
  Fl_Box* centralBox = new Fl_Box(128,128,100,100,nullptr);
  centralBox->box(FL_UP_BOX);
  mainViewport->end();
  //mainViewport->position(256, 256);
  //box->labelfont(FL_BOLD+FL_ITALIC);
  //box->labelsize(36);
  //box->labeltype(FL_SHADOW_LABEL);
  window->end();
  window->resizable(window);
  window->show(argc, argv);
  return Fl::run();
}
