#include <iostream>
#include <stack>
#include <tuple>
#include <vector>
using namespace std;

void printBinaryNumbers(unsigned n) {
  stack<tuple<unsigned,unsigned>> stk;
  vector<bool> result(n);
  stk.push({0,1});
  stk.push({0,0});
  while (not stk.empty()) {
    unsigned i, d;
    tie(i,d) = stk.top();
    stk.pop();
    result[i] = d;
    if (i == n-1) {
      for (unsigned j = 0; j < n; ++j) cout << result[j];
      cout << endl;
    }
    else {
      stk.push({i+1,1});
      stk.push({i+1,0});
    }
  }
}

int main() {
  printBinaryNumbers(4);
}


