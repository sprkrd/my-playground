#include <iostream>
#include <vector>
#include <string>
#include <vector>
#include <utility>
#include <set>
#include <map>
using namespace std;

const int inf = 1000000;

typedef pair<int,int> Cell;

Cell character_position(const vector<string>& world) {
  Cell retval(-1, -1);
  for (int y = 0; y < world.size(); ++y) {
    for (int x = 0; x < world[0].size(); ++x) {
      if (world[y][x] == '@') {
        retval.first = x;
        retval.second = y;
        break;
      };
    }
  }
  return retval;
}

void reachable_cells(const vector<string>& world, const Cell& current,
    set<Cell>& visited) {
  int size_x = world[0].size();
  int size_y = world.size();
  vector<Cell> neighbors;
  if (current.first > 0 and world[current.second][current.first-1] != 'X')
    neighbors.push_back(Cell(current.first-1, current.second));
  if (current.first < size_x-1 and world[current.second][current.first+1] != 'X')
    neighbors.push_back(Cell(current.first+1, current.second));
  if (current.second > 0 and world[current.second-1][current.first] != 'X')
    neighbors.push_back(Cell(current.first, current.second-1));
  if (current.second < size_y-1 and world[current.second+1][current.first] != 'X')
    neighbors.push_back(Cell(current.first, current.second+1));

  for (const Cell& neighbor : neighbors) {
    if (not visited.count(neighbor)) {
      visited.insert(neighbor);
      reachable_cells(world, neighbor, visited);
    }
  }
}

vector<Cell> reachable_cells(const vector<string>& world) {
  Cell char_pos = character_position(world);
  set<Cell> visited;
  visited.insert(char_pos);
  reachable_cells(world, char_pos, visited);
  return vector<Cell>(visited.begin(), visited.end());
}

map<Cell, int> cell_to_int_map(const vector<Cell>& cells) {
  map<Cell, int> ids;
  for (int idx = 0; idx < cells.size(); ++idx) {
    ids[cells[idx]] = idx;
  }
  return ids;
}

vector<vector<int>> adjacency_matrix(const vector<Cell>& cells) {
  int V = cells.size();
  auto cell_id_map = cell_to_int_map(cells);
  vector<vector<int>> adj(V, vector<int>(V, inf));
  for (int idx = 0; idx < V; ++idx) {
    adj[idx][idx] = 0;
    vector<Cell> adjacent {
      Cell(cells[idx].first+1, cells[idx].second),
      Cell(cells[idx].first-1, cells[idx].second),
      Cell(cells[idx].first, cells[idx].second+1),
      Cell(cells[idx].first, cells[idx].second-1),
    };
    for (const Cell& other_cell : adjacent) {
      auto it = cell_id_map.find(other_cell);
      if (it != cell_id_map.end()) adj[idx][it->second] = 1;
    }
  }
  return adj;
}

void floyd_warshall(vector<vector<int>>& matrix) {
  int V = matrix.size();
  for (int k = 0; k < V; ++k) {
    for (int i = 0; i < V; ++i) {
      for (int j = 0; j < V; ++j) {
        if (matrix[i][j] > matrix[i][k] + matrix[k][j]) {
          matrix[i][j] = matrix[i][k] + matrix[k][j];
        }
      }
    }
  }
}

ostream& operator<<(ostream& out, const vector<vector<int>>& matrix) {
  int V = matrix.size();
  auto flags0 = out.flags();
  for (int idx = 0; idx < V; ++idx) {
    for (int jdx = 0; jdx < V; ++jdx) {
      out.width(3);
      if (matrix[idx][jdx] == inf) out << "inf";
      else out << matrix[idx][jdx];
      if (jdx != V-1) {
        out.width(0);
        out << ' ';
      }
    }
    if (idx != V-1) out << endl; 
  }
  out.flags(flags0);
  return out;
}

int main() {
  vector<string> world;
  string line;
  while (getline(cin, line)) {
    world.push_back(line);
  }
  vector<Cell> reachable = reachable_cells(world);
  //for (const Cell& cell : reachable) {
    //cout << '(' << cell.first << ',' << cell.second << ')' << endl;
  //}
  auto adj = adjacency_matrix(reachable);
  floyd_warshall(adj);
  cout << adj << endl;
}

