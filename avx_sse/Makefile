a.out: test_load.cpp
	g++ -mavx -march=native -O3 -std=c++17 test_load.cpp -fopt-info-vec-optimized -ftree-vectorize -DUSE_AVX
