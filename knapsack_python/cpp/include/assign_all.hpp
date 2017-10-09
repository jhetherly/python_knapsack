#ifndef ASSIGN_ALL_HPP
#define ASSIGN_ALL_HPP

#include <cmath>
#include <limits>
#include "math_utils.hpp"


#include <iostream>


namespace mkp {

template<class ProfitT, class WeightT,
         class GroupT>
void filter_by_assignment(const ProfitT *p, const WeightT *w,
                          const GroupT *y, const GroupT filter_group,
                          const std::size_t n,
                          ProfitT *filtered_p,
                          WeightT *filtered_w,
                          std::size_t *filtered_indices) {

  std::size_t i_filtered = 0;
  for (std::size_t j = 0; j < n; ++j) {
    if (y[j] == filter_group) {
      filtered_p[i_filtered] = p[j];
      filtered_w[i_filtered] = w[j];
      filtered_indices[i_filtered] = j;
      ++i_filtered;
    }
  }
}


// TODO: since this is a direct translation of Numpy code there are likely
//       optimizations that can be made
template<class ProfitT, class WeightT,
         class GroupT>
ProfitT assign_all_impl(const ProfitT *p, const WeightT *w,
                        const std::size_t n,
                        const WeightT *c, const std::size_t m,
                        const double std_tol,
                        const std::size_t max_balance_iter,
                        ProfitT z,
                        GroupT *y_return,
                        const GroupT NO_GROUP) {
  /*
   * p:                 array of item profits
   * w:                 array of item weights
   * n:                 number of items
   * c:                 array of capacities
   * m:                 number of knapsacks
   * std_tol:           numerical tolerance on the difference of standard deviation
   * max_balance_iter:  maximum rebalancing iterations to run
   * z:                 initial profit
   * y_return:          array of group assignments (output)
   * NO_GROUP:          special group that represents no group
   */

  std::cout << "\nassign_all_impl";
  std::cout << "\np = ";
  for (std::size_t j = 0; j < n; ++j) {
    std::cout << p[j];
    if (j < n - 1) std::cout << ", ";
  }
  std::cout << "\nw = ";
  for (std::size_t j = 0; j < n; ++j) {
    std::cout << w[j];
    if (j < n - 1) std::cout << ", ";
  }
  std::cout << "\nc = ";
  for (std::size_t i = 0; i < m; ++i) {
    std::cout << c[i];
    if (i < m - 1) std::cout << ", ";
  }


  std::size_t number_of_unassigned_items = mkp::count_occurance(y_return, n, NO_GROUP);
  WeightT *packsizes = new WeightT[m];
  WeightT *knapsack_diffs = new WeightT[m];
  for (std::size_t i = 0; i < m; ++i) {
    WeightT capacity = 0;

    for (std::size_t j = 0; j < n; ++j)
      if (static_cast<GroupT>(i) == y_return[j]) capacity += w[j];

    packsizes[i] = capacity;
    knapsack_diffs[i] = c[i] - capacity;
  }

  // start rearranging knapsacks for items that were left out
  if (number_of_unassigned_items > 0) {
    ProfitT *unpacked_profits = new ProfitT[number_of_unassigned_items];
    WeightT *unpacked_weights = new WeightT[number_of_unassigned_items];
    std::size_t *unpacked_indices = new std::size_t[number_of_unassigned_items];

    filter_by_assignment(p, w, y_return, NO_GROUP, n,
                         unpacked_profits, unpacked_weights, unpacked_indices);

    // overstuff knapsacks with the most (relative) room with the leftover items
    for (std::size_t k = 0; k < number_of_unassigned_items; ++k) {
      WeightT unpacked_weight = unpacked_weights[k];
      std::size_t min_knapsack_index = 0;
      for (std::size_t i = 0; i < m; ++i) {
        WeightT knapsack_diff = knapsack_diffs[i];

        if (std::fabs(knapsack_diff - unpacked_weight)/c[i] <
            std::fabs(knapsack_diffs[min_knapsack_index] - unpacked_weight)/c[min_knapsack_index]) {
          min_knapsack_index = i;
        }
      }
      y_return[unpacked_indices[k]] = min_knapsack_index;
      packsizes[min_knapsack_index] += unpacked_weight;
      z += unpacked_profits[k];
      knapsack_diffs[min_knapsack_index] = c[min_knapsack_index] - packsizes[min_knapsack_index];
    }

    // make to knapsacks share the loads more evenly by reducing the variance
    // of the relative differences of the knapsacks and their total capacities
    double *relative_diffs = new double[m];
    for (std::size_t i = 0; i < m; ++i) relative_diffs[i] = static_cast<double>(knapsack_diffs[i])/c[i];

    if (mkp::sample_std<double>(relative_diffs, m) > std_tol) {
      std::size_t iter_count = 0;

      while (iter_count < max_balance_iter) {
        // find most overstuffed knapsack
        std::size_t most_overstuffed_knapsack_index = mkp::argmin(relative_diffs, m);
        std::size_t number_in_most_overstuffed_knapsack = mkp::count_occurance(y_return, n, most_overstuffed_knapsack_index);
        // isolate these items and sort them by weight
        ProfitT *most_overstuffed_knapsack_profits = new ProfitT[number_in_most_overstuffed_knapsack];
        WeightT *most_overstuffed_knapsack_weights = new WeightT[number_in_most_overstuffed_knapsack];
        std::size_t *most_overstuffed_knapsack_indices = new std::size_t[number_in_most_overstuffed_knapsack];

        filter_by_assignment(p, w, y_return,
                             static_cast<GroupT>(most_overstuffed_knapsack_index), n,
                             most_overstuffed_knapsack_profits,
                             most_overstuffed_knapsack_weights,
                             most_overstuffed_knapsack_indices);

        std::vector<std::size_t> argsort_most_overstuffed_knapsack_weights =
          mkp::argsort_ascending(most_overstuffed_knapsack_weights, number_in_most_overstuffed_knapsack);
        WeightT *most_overstuffed_knapsack_weights_sorted = new WeightT[number_in_most_overstuffed_knapsack];
        std::size_t *most_overstuffed_knapsack_indices_sorted = new std::size_t[number_in_most_overstuffed_knapsack];
        for (std::size_t i = 0; i < number_in_most_overstuffed_knapsack; ++i) {
          most_overstuffed_knapsack_weights_sorted[i] = most_overstuffed_knapsack_weights[argsort_most_overstuffed_knapsack_weights[i]];
          most_overstuffed_knapsack_indices_sorted[i] = most_overstuffed_knapsack_indices[argsort_most_overstuffed_knapsack_weights[i]];
        }

        std::size_t best_weight_index = 0;
        std::size_t best_sub_weight_index = 0;
        std::size_t best_sub_knapsack_index = 0;
        double prev_ave_rel_diff = std::numeric_limits<double>::max();
        for (std::size_t weight_index = 0; weight_index < number_in_most_overstuffed_knapsack; ++weight_index) {
          WeightT overstuffed_weight_sum = 0;
          for (std::size_t wi = 0; wi < weight_index; ++wi) overstuffed_weight_sum += most_overstuffed_knapsack_weights_sorted[wi];

          for (std::size_t knapsack_index = 0; knapsack_index < m; ++knapsack_index) {
            if (knapsack_index == most_overstuffed_knapsack_index) continue;
            // isolate these items and sort them by weight
            std::size_t number_in_knapsack = mkp::count_occurance(y_return, n, knapsack_index);
            ProfitT *knapsack_profits = new ProfitT[number_in_knapsack];
            WeightT *knapsack_weights = new WeightT[number_in_knapsack];
            std::size_t *knapsack_indices = new std::size_t[number_in_knapsack];

            filter_by_assignment(p, w, y_return,
                                 static_cast<GroupT>(knapsack_index), n,
                                 knapsack_profits,
                                 knapsack_weights,
                                 knapsack_indices);
            std::vector<std::size_t> argsort_knapsack_weights =
              mkp::argsort_ascending(knapsack_weights, number_in_knapsack);
            WeightT *knapsack_weights_sorted = new WeightT[number_in_knapsack];
            for (std::size_t i = 0; i < number_in_knapsack; ++i)
              knapsack_weights_sorted[i] = knapsack_weights[argsort_knapsack_weights[i]];

            for (std::size_t sub_weight_index = 0; sub_weight_index < number_in_knapsack; ++sub_weight_index) {
              WeightT weight_sum = 0;
              for (std::size_t wi = 0; wi < sub_weight_index; ++wi) weight_sum += knapsack_weights_sorted[wi];

              double cur_ave_rel_diff = 0.0;
              cur_ave_rel_diff += std::fabs(knapsack_diffs[most_overstuffed_knapsack_index] + overstuffed_weight_sum - weight_sum)/c[most_overstuffed_knapsack_index];
              cur_ave_rel_diff += std::fabs(knapsack_diffs[knapsack_index] - overstuffed_weight_sum + weight_sum)/c[knapsack_index];
              cur_ave_rel_diff *= 0.5;

              if (cur_ave_rel_diff < prev_ave_rel_diff) {
                best_weight_index = weight_index;
                best_sub_weight_index = sub_weight_index;
                best_sub_knapsack_index = knapsack_index;
                prev_ave_rel_diff = cur_ave_rel_diff;
              }
            }
            delete [] knapsack_weights_sorted;
            delete [] knapsack_profits;
            delete [] knapsack_weights;
            delete [] knapsack_indices;
          }
        }

        std::size_t number_in_knapsack = mkp::count_occurance(y_return, n, best_sub_knapsack_index);
        ProfitT *knapsack_profits = new ProfitT[number_in_knapsack];
        WeightT *knapsack_weights = new WeightT[number_in_knapsack];
        std::size_t *knapsack_indices = new std::size_t[number_in_knapsack];

        filter_by_assignment(p, w, y_return,
                             static_cast<GroupT>(best_sub_knapsack_index), n,
                             knapsack_profits,
                             knapsack_weights,
                             knapsack_indices);
        std::vector<std::size_t> argsort_knapsack_weights =
          mkp::argsort_ascending(knapsack_weights, number_in_knapsack);
        WeightT *knapsack_weights_sorted = new WeightT[number_in_knapsack];
        std::size_t *knapsack_indices_sorted = new std::size_t[number_in_knapsack];
        for (std::size_t i = 0; i < number_in_knapsack; ++i) {
          knapsack_weights_sorted[i] = knapsack_weights[argsort_knapsack_weights[i]];
          knapsack_indices_sorted[i] = knapsack_indices[argsort_knapsack_weights[i]];
        }

        for (std::size_t j = 0; j < n; ++j) {
          for (std::size_t i = 0; i < best_weight_index; ++i) {
            if (most_overstuffed_knapsack_indices_sorted[i] == j)
              y_return[j] = best_sub_knapsack_index;
          }
          for (std::size_t i = 0; i < best_sub_weight_index; ++i) {
            if (knapsack_indices_sorted[i] == j)
              y_return[j] = most_overstuffed_knapsack_index;
          }
        }

        for (std::size_t i = 0; i < m; ++i) {
          packsizes[i] = 0;
          for (std::size_t j = 0; j < n; ++j)
            if (y_return[j] == static_cast<GroupT>(i))
              packsizes[i] += w[j];
        }
        for (std::size_t i = 0; i < m; ++i) {
          knapsack_diffs[i] = c[i] - packsizes[i];
          relative_diffs[i] = static_cast<double>(knapsack_diffs[i])/c[i];
        }

        if (mkp::sample_std<double>(relative_diffs, m) > std_tol) break;

        ++iter_count;

        delete [] knapsack_indices_sorted;
        delete [] knapsack_weights_sorted;
        delete [] knapsack_profits;
        delete [] knapsack_weights;
        delete [] knapsack_indices;
        delete [] most_overstuffed_knapsack_weights_sorted;
        delete [] most_overstuffed_knapsack_indices_sorted;
        delete [] most_overstuffed_knapsack_profits;
        delete [] most_overstuffed_knapsack_weights;
        delete [] most_overstuffed_knapsack_indices;
      }
    }

    delete [] relative_diffs;
    delete [] unpacked_profits;
    delete [] unpacked_weights;
    delete [] unpacked_indices;
  }


  delete [] packsizes;
  delete [] knapsack_diffs;

  return z;
}


}

#endif
