#ifndef MTHG_HPP
#define MTHG_HPP


#include <limits>
#include <set>


namespace gap {

template<class ProfitT, class WeightT,
         class AffinityT, class GroupT>
ProfitT mthg_impl(const ProfitT *p, const WeightT *w, const AffinityT *f,
                  const std::size_t n,
                  const WeightT *c, const std::size_t m,
                  GroupT *y_return,
                  const GroupT NO_GROUP,
                  const bool run_improvement = true) {
  /*
   * p:         (flattened) array of item profits
   * w:         (flattened) array of item weights
   * f:         (flattened) array of item desirabilities
   * n:         number of items
   * c:         array of capacities
   * m:         number of knapsacks
   * y_return:  array of group assignments (output)
   * NO_GROUP:  special group that represents no group
   * run_improvement:  flag to indicate whether or not to run the
   *                   improvement step
   */

  std::set<std::size_t> M, U;
  for (std::size_t i = 0; i < m; ++i)
    M.insert(i);
  for (std::size_t j = 0; j < n; ++j) {
    U.insert(j);
    y_return[j] = NO_GROUP;
  }

  // initial solution
  bool feas = true;
  WeightT *c_bar = new WeightT[m];
  for (std::size_t i = 0; i < m; ++i) c_bar[i] = c[i];
  ProfitT z = 0;

  while (!U.empty() && feas) {
    AffinityT d_star = -std::numeric_limits<AffinityT>::max();
    std::size_t i_star = 0,
                j_star = 0;

    for (std::set<std::size_t>::iterator j = U.begin(); j != U.end(); ++j) {
      std::set<std::size_t> F_j;
      for (std::set<std::size_t>::iterator i = M.begin(); i != M.end(); ++i)
        if (w[(*i)*n + (*j)] <= c_bar[*i]) F_j.insert(*i);

      feas = !F_j.empty();
      if (feas) {
        std::size_t i_prime = 0;
        AffinityT max_f = -std::numeric_limits<AffinityT>::max(),
                  max2_f = -std::numeric_limits<AffinityT>::max();
        for (std::set<std::size_t>::iterator i = F_j.begin(); i != F_j.end(); ++i) {
          if (f[(*i)*n + (*j)] > max_f) {
            i_prime = *i;
            max2_f = max_f;
            max_f = f[(*i)*n + (*j)];
          }
          else if (f[(*i)*n + (*j)] > max2_f)
            max2_f = f[(*i)*n + (*j)];
        }

        AffinityT d = std::numeric_limits<AffinityT>::max();
        if (F_j.size() > 1) d = max_f - max2_f;
        if (d > d_star) {
          d_star = d;
          i_star = i_prime;
          j_star = *j;
        }
      }
    }

    if (feas) {
      y_return[j_star] = static_cast<GroupT>(i_star);
      z += p[i_star*n + j_star];
      c_bar[i_star] -= w[i_star*n + j_star];
      U.erase(j_star);
    }
  }

  // improvement
  if (feas && run_improvement) {
    for (std::size_t j = 0; j < n; ++j) {
      GroupT i_prime = y_return[j];
      std::set<ProfitT> A;
      std::size_t i_double_prime;
      ProfitT p_i_double_prime_j = std::numeric_limits<ProfitT>::min();

      for (std::size_t i = 0; i < m; ++i) {
        if (static_cast<GroupT>(i) != i_prime && w[i*n + j] <= c_bar[i]) {
          A.insert(p[i*n + j]);
          if (p[i*n + j] > p_i_double_prime_j) {
            p_i_double_prime_j = p[i*n + j];
            i_double_prime = i;
          }
        }
      }

      if (!A.empty() && p_i_double_prime_j > p[i_prime*n + j]) {
        y_return[j] = i_double_prime;
        z += p_i_double_prime_j - p[i_prime*n + j];
        c_bar[i_prime] += w[i_prime*n + j];
        c_bar[i_double_prime] -= w[i_double_prime*n + j];
      }
    }
  }

  delete [] c_bar;

  return z;
}

}


#endif
