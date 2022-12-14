#include <iostream>
#include <ctime>
#include <cmath>
#include <array>
using namespace std;


template<int N>
array<bool, 1<<N> luSqrtLastDigits() {
  array<bool, 1<<N> lu;
  lu.fill(false);
  for (int i = 0; i < 1<<N; ++i) {
    int i_sqr = i*i;
    lu[i_sqr&((1<<N) - 1)] = true;
  }
  //for (unsigned i = 0; i < lu.size(); ++i) {
    //cout << i << ' ' << lu[i] << endl;
  //}
  return lu;
}


bool isPerfectSquareBisection(int n) {
  static auto lu_sqrt_last_digits = luSqrtLastDigits<6>();

  if (not lu_sqrt_last_digits[n&(lu_sqrt_last_digits.size()-1)]) {
    return false;
  }

  int left = 0;
  int right = 1;
  while (right*right < n) {
    left = right;
    right <<= 1;
  }

  while (left <= right) {
    int middle = (left + right) / 2;
    if (middle*middle < n) {
      left = middle + 1;
    }
    else if (middle*middle > n) {
      right = middle - 1;
    }
    else {
      return true;
    }
  }
  return false;
}


bool isPerfectSquareNewton(int n) {
  static auto lu_sqrt_last_digits = luSqrtLastDigits<6>();

  if (not lu_sqrt_last_digits[n&(lu_sqrt_last_digits.size()-1)]) {
    return false;
  }

  //int x = 1;
  //while (n/x > x) {
    //x <<= 1;
  //}

  int left = 0;
  int right = 1;
  while (right*right < n) {
    left = right;
    right <<= 1;
  }
  int x = (left+right+1)/2;

  bool converged = false;
  while (not converged) {
    int x_next = (x+n/x)/2;
    converged = abs(x_next-x) <= 1;
    x = x_next;
  }
  return x*x == n;
}


bool isPerfectSquareMathSqrt(int n) {
  //static auto lu_sqrt_last_digits = luSqrtLastDigits<6>();

  //if (not lu_sqrt_last_digits[n&(lu_sqrt_last_digits.size()-1)]) {
    //return false;
  //}
  int sqrt_n = (int)sqrt(n);
  return sqrt_n*sqrt_n == n;
}


int main() {
  //cout << "hello" << endl;
  //cout << isPerfectSquareBisection(100) << endl;


  for (int i = 0; i <= 10000000; ++i) {
    bool perfect1 = isPerfectSquareBisection(i);
    bool perfect2 = isPerfectSquareNewton(i);
    bool perfect3 = isPerfectSquareMathSqrt(i);
    if ((perfect1^perfect2)||(perfect1^perfect3)) {
      cout << i << endl;
    }
  }

  int count;

  time_t start = clock();
  count = 0;
  for (int i = 0; i <= 10000000; ++i) {
    count += isPerfectSquareNewton(i);
  }
  double elapsed_ms = 100.0*(clock() - start)/CLOCKS_PER_SEC;
  cout << elapsed_ms << "ns" << endl;

  start = clock();
  count = 0;
  for (int i = 0; i <= 10000000; ++i) {
    count += isPerfectSquareBisection(i);
  }
  elapsed_ms = 100.0*(clock() - start)/CLOCKS_PER_SEC;
  cout << elapsed_ms << "ns" << endl;

  start = clock();
  count = 0;
  for (int i = 0; i <= 10000000; ++i) {
    count += isPerfectSquareMathSqrt(i);
  }
  elapsed_ms = 100.0*(clock() - start)/CLOCKS_PER_SEC;
  cout << elapsed_ms << "ns" << endl;
}
