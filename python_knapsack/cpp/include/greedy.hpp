#ifndef GREEDY_HPP
#define GREEDY_HPP

#include <cstddef>


namespace mkp {


template<class ProfitT, class WeightT,
         class GroupT>
ProfitT greedy(const ProfitT *p, const WeightT *w,
               const std::size_t n, const std::size_t i,
               WeightT *c_bar, GroupT *y,
               const GroupT NO_GROUP) {
  /*
   * p:         array of item profits
   * w:         array of item weights
   * n:         number of items
   * i:         current group index
   * c_bar:     array of remaining capacities
   * y:         array of group assignments
   * NO_GROUP:  special group that represents no group
   */

  const GroupT group_id = static_cast<const GroupT>(i);
  ProfitT z = 0;
  for (std::size_t j = 0; j < n; ++j) {
    if (y[j] == NO_GROUP && w[j] <= c_bar[i]) {
      y[j] = group_id;
      c_bar[i] -= w[j];
      z += p[j];
    }
  }

  return z;
}


}

#endif
