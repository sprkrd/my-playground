#include <iostream>
#include <ctime>
#include <cmath>
using namespace std;

bool isPerfectSquareMathSqrt(unsigned long long n) {
  unsigned long long sqrt_n = (unsigned long long)sqrt(n);
  return sqrt_n*sqrt_n == n;
}


int main() {
  unsigned long long n;
  cin >> n;
  for (unsigned long long i = 0; i <= n; ++i) {
    if (isPerfectSquareMathSqrt(8*i*i+1)) {
      cout << i << endl;
    }
  }
}
