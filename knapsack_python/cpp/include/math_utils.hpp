#ifndef MATH_UTILS_HPP
#define MATH_UTILS_HPP

#include <cstddef>
#include <cmath>
#include <vector>
#include <algorithm>


namespace mkp {

template<class ValueT, class ArrayT>
ValueT sample_std(const ArrayT &x, const std::size_t &n) {
  if (n < 2) return 0;

  ValueT prev_A = 0,
         prev_Q = 0,
         A, Q, diff;
  for (std::size_t i = 0; i < n; ++i) {
    diff = x[i] - prev_A;
    A = prev_A + (diff)/(i + 1);
    Q = prev_Q + static_cast<ValueT>(i)/(i + 1)*diff*diff;
    prev_A = A;
    prev_Q = Q;
  }

  return std::sqrt(Q/static_cast<ValueT>(n - 1));
}


template<class ValueT, class ArrayT>
std::size_t count_occurance(const ArrayT &x, const std::size_t &n, const ValueT & val) {
  std::size_t count = 0;

  for (std::size_t i = 0; i < n; ++i)
    if (static_cast<ValueT>(x[i]) == val) ++count;

  return count;
}


template<class ArrayT>
std::size_t argmin(const ArrayT x, const std::size_t &n) {
  std::size_t current_min_index = 0;

  for (std::size_t i = 1; i < n ; ++i)
    if (x[i] < x[current_min_index])
      current_min_index = i;

  return current_min_index;
}


template<class ArrayT>
std::size_t argmax(const ArrayT x, const std::size_t &n) {
  std::size_t current_max_index = 0;

  for (std::size_t i = 1; i < n ; ++i)
    if (x[i] > x[current_max_index])
      current_max_index = i;

  return current_max_index;
}


template<class ArrayT>
struct DescendingArgSort {
  DescendingArgSort(const ArrayT &a_) : a(a_) {}
  bool operator() (const std::size_t &i, const std::size_t & j) {
    return (a[i] > a[j]);
  }
  const ArrayT &a;
};


template<class ArrayT>
struct AscendingArgSort {
  AscendingArgSort(const ArrayT &a_) : a(a_) {}
  bool operator() (const std::size_t &i, const std::size_t & j) {
    return (a[i] < a[j]);
  }
  const ArrayT &a;
};


template<class ArrayT>
std::vector<std::size_t> argsort_descending(const ArrayT &a, const std::size_t &n) {
  std::vector<std::size_t> sorted_args(n);

  for (std::size_t j = 0; j < n; ++j) sorted_args[j] = j;
  std::stable_sort(sorted_args.begin(), sorted_args.end(),
                   DescendingArgSort<ArrayT>(a));

  return sorted_args;
}


template<class ArrayT>
std::vector<std::size_t> argsort_ascending(const ArrayT &a, const std::size_t &n) {
  std::vector<std::size_t> sorted_args(n);

  for (std::size_t j = 0; j < n; ++j) sorted_args[j] = j;
  std::stable_sort(sorted_args.begin(), sorted_args.end(),
                   AscendingArgSort<ArrayT>(a));

  return sorted_args;
}

}


#endif
