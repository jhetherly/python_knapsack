#include <iostream>
#include <map>

#include "fuzzy_mthm.hpp"


int main() {
  std::size_t n, m;

  n = 10;
  m = 3;
  int *c = new int[m];
  int *p = new int[n];
  int *w = new int[n];
  int *y = new int[n];

  w[0] = 2;
  w[1] = 2;
  w[2] = 1;
  w[3] = 4;
  w[4] = 2;
  w[5] = 8;
  w[6] = 5;
  w[7] = 2;
  w[8] = 1;
  w[9] = 4;

  for (std::size_t j = 0; j < n; ++j) {
    p[j] = 1;
    // p[j] = w[j];
  }

  c[0] = 13;
  c[1] = 1;
  c[2] = 17;

  std::map<int, int> knapsack_weights;

  int z = mkp::fuzzy_mthm_impl(p, w, n, c, m, 1e-3, 100, 100, y, -1);
  std::cout << "total score: " << z << std::endl;
  for (std::size_t j = 0; j < n; ++j) {
    std::cout << j << ": " << y[j] << std::endl;
    knapsack_weights[y[j]] += w[j];
  }

  for (std::size_t i = 0; i < m; ++i) {
    std::cout << i << ": " << c[i] << " -> " << knapsack_weights[i] << std::endl;
  }

  return 0;
}
