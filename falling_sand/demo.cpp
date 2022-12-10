#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <curses.h>
using namespace std;

constexpr int k_width  = 32;
constexpr int k_height = 32;

void update(vector<string>& map) {
  vector<string> buffer(k_height, string(k_width,' '));
  int x_inc = -1;
  for (int y = 0; y < k_height-1; ++y) {
    for (int x = 0; x < k_width; ++x) {
      if (map[y][x] == '.') {
        if (map[y+1][x] == ' ')
          buffer[y+1][x] = '.';
        else if (x+x_inc>=0 && x+x_inc<k_width && map[y+1][x+x_inc]==' ') {
          buffer[y+1][x+x_inc] = '.';
          x_inc *= -1;
        }
        else if (x-x_inc>=0 && x-x_inc<k_width && map[y+1][x-x_inc]==' ')
          buffer[y+1][x-x_inc] = '.';
        else
          buffer[y][x] = '.';
      }
    }
  }
  for (int x = 0; x < k_width; ++x) {
    if (map[k_height-1][x] == '.')
      buffer[k_height-1][x] = '.';
  }
  buffer[0][k_width/2] = '.';
  buffer.swap(map);
}

ostream& operator<<(ostream& out, const vector<string>& map) {
  for (unsigned i = 0; i < map.size(); ++i) {
    if (i > 0) out << '\n';
    out << map[i];
  }
  return out;
}

void output_map(WINDOW* win, const vector<string>& map) {
  wmove(win,0,0);
  for (const string& row : map) {
    for (char c : row)
      waddch(win,c);
  }
  wrefresh(win);
}

int main() {
  initscr();
  WINDOW* win = newwin(k_height, k_width, 0, 0);

  vector<string> map(k_height, string(k_width,' '));
  output_map(win,map);
  for (int i = 0; i < 500; ++i) {
    update(map);
    this_thread::sleep_for(chrono::milliseconds(100));
    output_map(win,map);
  }
  delwin(win);
  endwin();
}

