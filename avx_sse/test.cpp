#pragma GCC optimize("O3","unroll-loops","omit-frame-pointer","inline") //Optimization flags
#pragma GCC option("arch=native","tune=native","no-zeroupper") //Enable AVX
#pragma GCC target("avx")  //Enable AVX

#include <immintrin.h>
#include <vector>
#include <iostream>
#include <cstddef>
#include <vector>
#include <random>
#include <limits>
#include <cmath>
#include <chrono>
#include <new>
using namespace std;
using namespace std::chrono;


int main() {
  mt19937 rng(42);

  int N = 64000000;
  int V = N/8;
  float c = 1.0;
  float log_p = log(100000);

  normal_distribution<float> normal(0,10);
  uniform_int_distribution<> unif(1,1000);

  float maxvalue = -numeric_limits<float>::infinity();
  int argmax = -1;

#ifdef USE_AVX
  auto* v = new(align_val_t(32)) __m256[V];
  auto* n = new(align_val_t(32)) __m256[V];
  for (int i = 0; i < V; ++i) {
    for (int j = 0; j < 8; ++j) {
      v[i][j] = normal(rng);
      n[i][j] = unif(rng);
    }
  }
#else
  vector<float> v(N);
  vector<float> n(N);
  for (int i = 0; i < N; ++i) {
    v[i] = normal(rng);
    n[i] = unif(rng);
  }
#endif

  auto start = steady_clock::now();

#ifdef USE_AVX
  __m256 C = _mm256_set1_ps(c*sqrt(log_p));
  for (int i = 0; i < V; ++i) {
    __m256 score = _mm256_add_ps(v[i], _mm256_mul_ps(C, _mm256_rsqrt_ps(n[i])));
    for (int j = 0; j < 8; ++j) {
      if (score[j] > maxvalue) {
        maxvalue = score[j];
        argmax = i*8 + j;
      }
    }
  }
#else
  float C = c*sqrt(log_p);
  for (int i = 0; i < N; ++i) {
    float score = v[i] + C/sqrt(n[i]);
    if (score > maxvalue) {
      maxvalue = score;
      argmax = i;
    }
  }
#endif

  double elapsed_ms = 1000*duration<double>(steady_clock::now() - start).count();

#ifdef USE_AVX
  delete[] v;
  delete[] n;
#endif

  cout << maxvalue << ' ' << argmax << endl;
  cout << "Elapsed (ms): " << elapsed_ms << endl;

}


