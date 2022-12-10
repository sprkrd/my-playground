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


constexpr float k_inf = std::numeric_limits<float>::infinity();


int main() {
  mt19937 rng(42);

  int N = 6400000;
  int V = N/8;
  float c = 1.0;
  float log_p = log(100000);

  normal_distribution<float> normal(0,10);
  uniform_int_distribution<> unif(1,1000);

  float maxvalue = -numeric_limits<float>::infinity();
  int argmax = -1;

  vector<float> v(N);
  vector<float> n(N);
  for (int i = 0; i < N; ++i) {
    v[i] = normal(rng);
    n[i] = unif(rng);
  }

  auto start = steady_clock::now();

#ifdef USE_AVX
  __m256 C = _mm256_set1_ps(c*sqrt(log_p));
  __m256 mmax = _mm256_set1_ps(-k_inf);
  __m256 mmax_i = _mm256_setzero_ps();
  for (int i = 0; i < N; i+=8) {
    __m256 x = _mm256_loadu_ps(&v[i]);
    __m256 y = _mm256_loadu_ps(&n[i]);
    __m256 score = _mm256_add_ps(x, _mm256_mul_ps(C, _mm256_rsqrt_ps(y)));
    __m256 mask = _mm256_cmp_ps(score, mmax, 1);
    mmax = _mm256_blendv_ps(score, mmax, mask);
    mmax_i = _mm256_blendv_ps(_mm256_set1_ps(float(i)), mmax_i, mask);
    //for (int j = 0; j < 8; ++j) {
      //if (score[j] > maxvalue) {
        //maxvalue = score[j];
        //argmax = i*8 + j;
      //}
    //}
  }

  for (int i = 0; i < 8; ++i) {
    if (mmax[i] > maxvalue) {
      maxvalue = mmax[i];
      argmax = (int)mmax_i[i] + i;
    }
  }

#else
  float C = c*sqrt(log_p);
  for (int i = 0; i < N; ++i) {
    float score = n[i]? (v[i]+C/sqrt(n[i])) : k_inf;
    if (score > maxvalue) {
      maxvalue = score;
      argmax = i;
    }
  }
#endif

  double elapsed_ms = 1000*duration<double>(steady_clock::now() - start).count();


  cout << maxvalue << ' ' << argmax << endl;
  cout << "Elapsed (ms): " << elapsed_ms << endl;

}


