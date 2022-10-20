#include <iostream>
#include <vector>
using namespace std;

const int N = 1000;

int main() {
  vector<int> last_seen(N+1, -1);
  cout << "n,a(n)\n0,0\n";
  int a_n_prev = 0;
  for (int n = 1; n <= N; ++n) {
    int a_n = last_seen[a_n_prev] == -1? 0 : n - last_seen[a_n_prev] - 1;
    last_seen[a_n_prev] = n - 1;
    a_n_prev = a_n;
    cout << n << ',' << a_n << '\n';
  }
}
