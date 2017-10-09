#ifndef MTHM_HPP
#define MTHM_HPP

#include <cstddef>
#include <limits>
#include <algorithm>
#include <vector>
#include <set>

#include "math_utils.hpp"
#include "greedy.hpp"


namespace mkp {

template<class ProfitT, class WeightT,
         class GroupT>
ProfitT mthm_impl(const ProfitT *p, const WeightT *w,
                  const std::size_t n,
                  const WeightT *c, const std::size_t m,
                  const std::size_t min_iter,
                  GroupT *y_return,
                  const GroupT NO_GROUP) {
  /*
   * p:         array of item profits
   * w:         array of item weights
   * n:         number of items
   * c:         array of capacities
   * m:         number of knapsacks
   * min_iter:  minimum number of improvement trials to run
   * y_return:  array of group assignments (output)
   * NO_GROUP:  special group that represents no group
   */

  std::vector<std::size_t> c_sorted_args = mkp::argsort_ascending(c, m);

  WeightT *c_sorted = new WeightT[m];
  for (std::size_t i = 0; i < m; ++i) c_sorted[i] = c[c_sorted_args[i]];

  ProfitT total_profit = 0;
  std::vector<double> p_w(n);
  for (std::size_t j = 0; j < n; ++j) {
    p_w[j] = static_cast<double>(p[j])/static_cast<double>(w[j]);
    total_profit += p[j];
  }
  std::vector<std::size_t> pw_sorted_args = mkp::argsort_descending(p_w, n);

  ProfitT *p_sorted = new ProfitT[n];
  WeightT *w_sorted = new WeightT[n];
  GroupT *y = new GroupT[n];
  for (std::size_t j = 0; j < n; ++j) {
    p_sorted[j] = p[pw_sorted_args[j]];
    w_sorted[j] = w[pw_sorted_args[j]];
    y[j] = NO_GROUP;
  }

  WeightT *c_bar = new WeightT[m];
  for (std::size_t i = 0; i < m; ++i) {
    c_bar[i] = c_sorted[i];
  }

  ProfitT z = 0;
  for (std::size_t i = 0; i < m; ++i)
    z += mkp::greedy(p_sorted, w_sorted, n, i, c_bar, y, NO_GROUP);

  if (z < total_profit) {
    // rearrangement
    z = 0;
    for (std::size_t i = 0; i < m; ++i) {
      c_bar[i] = c_sorted[i];
    }

    std::size_t i = 0;
    for (long j = n - 1; j >= 0; --j) {
      if (y[j] != NO_GROUP) {
        std::size_t l = 0;
        bool found_l = false;
        for (std::size_t ll = i; ll < m; ++ll) {
          if (w_sorted[j] <= c_bar[ll]) {
            l = ll;
            break;
          }
        }
        if (!found_l) {
          for (std::size_t ll = 0; ll < i; ++ll) {
            if (w_sorted[j] <= c_bar[ll]) {
              l = ll;
              break;
            }
          }
        }
        if (!found_l) {
          y[j] = NO_GROUP;
        }
        else {
          y[j] = l;
          c_bar[l] -= w_sorted[j];
          z += p_sorted[j];
          if (l < m - 1) i = l + 1;
          else i = 0;
        }
      }
    }
    for (std::size_t i = 0; i < m; ++i)
      z += mkp::greedy(p_sorted, w_sorted, n, i, c_bar, y, NO_GROUP);

    // first improvement
    for (std::size_t j = 0; j < n; ++j) {
      if (y[j] != NO_GROUP) {
        for (std::size_t k = j + 1; k < n; ++k) {
          if (y[k] != NO_GROUP and y[k] != y[j]) {
            std::size_t h, l;
            if (w_sorted[j] > w_sorted[k]) {
              h = j;
              l = k;
            }
            else {
              h = k;
              l = j;
            }

            WeightT d = w_sorted[h] - w_sorted[l];
            WeightT min_null_weight = std::numeric_limits<WeightT>::max();
            for (std::size_t u = 0; u < n; ++u) {
              if (y[u] == NO_GROUP && min_null_weight > w_sorted[u])
                min_null_weight = w_sorted[u];
            }

            if (d <= c_bar[y[l]] && c_bar[y[h]] + d >= min_null_weight) {
              ProfitT max_null_profit = std::numeric_limits<ProfitT>::min();
              std::size_t t = 0;
              for (std::size_t u = 0; u < n; ++u) {
                if (y[u] == NO_GROUP && w_sorted[u] <= d + c_bar[y[h]] && p_sorted[u] > max_null_profit) {
                  max_null_profit = p_sorted[u];
                  t = u;
                }
              }
              c_bar[y[h]] += d - w_sorted[t];
              c_bar[y[l]] -= d;
              GroupT temp_yt = y[t];
              y[t] = y[h];
              y[h] = y[l];
              y[l] = temp_yt;
              z += p_sorted[t];
            }
          }
        }
      }
    }

    // repeat second improvement until no further improvement is found
    std::size_t iter_count = 0;
    while (true) {
      ProfitT z_prev = z;
      ++iter_count;

      // second improvement
      for (long j = n - 1; j >= 0; --j) {
        if (y[j] != NO_GROUP) {
          WeightT c_bar_temp = c_bar[y[j]] + w_sorted[j];
          std::set<std::size_t> Y = std::set<std::size_t>();
          for (std::size_t k = 0; k < n; ++k) {
            if (y[k] == NO_GROUP && w_sorted[k] <= c_bar_temp) {
              Y.insert(k);
              c_bar_temp -= w_sorted[k];
            }
          }
          ProfitT pk_sum = 0;
          for (std::set<std::size_t>::iterator it = Y.begin(); it != Y.end(); ++it)
            pk_sum += p_sorted[*it];
          if (pk_sum > p_sorted[j]) {
            for (std::set<std::size_t>::iterator it = Y.begin(); it != Y.end(); ++it) {
              y[*it] = y[j];
              c_bar[y[j]] = c_bar_temp;
              y[j] = NO_GROUP;
              z += pk_sum - p_sorted[j];
            }
          }
        }
      }
      if (iter_count >= min_iter && z_prev == z) break;
    }
  }

  // undo sorting
  for (std::size_t j = 0; j < n; ++j) {
    if (y[j] != NO_GROUP)
      y_return[pw_sorted_args[j]] = c_sorted_args[y[j]];
    else
      y_return[pw_sorted_args[j]] = NO_GROUP;
  }

  delete [] c_sorted;
  delete [] p_sorted;
  delete [] w_sorted;
  delete [] y;
  delete [] c_bar;

  return z;
}

}


#endif
