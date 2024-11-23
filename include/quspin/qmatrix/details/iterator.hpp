// Copyright 2024 Phillip Weinberg
#pragma once

#include <algorithm>
#include <quspin/details/type_concepts.hpp>

namespace quspin {
namespace details {

template <typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
struct row_info {
 public:
  T &value;
  I &index;
  J &cindex;

  row_info(T &value, I &index, J &cindex)
      : value(value), index(index), cindex(cindex) {};
  row_info(row_info<T, I, J> &other)
      : value(other.value), index(other.index), cindex(other.cindex) {};
  row_info(row_info<T, I, J> &&other)
      : value(other.value), index(other.index), cindex(other.cindex) {};

  row_info &operator=(row_info<T, I, J> &other);
  row_info &operator=(row_info<T, I, J> &&other);
  bool operator<(const row_info &other) const;
  bool operator>(const row_info &other) const;
  bool operator==(const row_info &other) const;
  bool operator!=(const row_info &other) const;
  bool operator<=(const row_info &other) const;
  bool operator>=(const row_info &other) const;
};

static_assert(std::swappable<row_info<int8_t, int32_t, uint8_t>>,
              "row_info is not nothrow swappable");

template <typename T, typename I, typename J>
void swap(row_info<T, I, J> lhs, row_info<T, I, J> rhs) {
  std::swap(lhs.value, rhs.value);
  std::swap(lhs.index, rhs.index);
  std::swap(lhs.cindex, rhs.cindex);
}

template <typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
struct iterator {
 private:
  T *data_ = nullptr;
  I *indices_ = nullptr;
  J *cindices_ = nullptr;

 public:
  using value_type = row_info<T, I, J>;
  using reference_type = row_info<T, I, J>;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::random_access_iterator_tag;
  using pointer_type = row_info<T, I, J> *;

  iterator() {};
  iterator(T *data, I *indices, J *cindices)
      : data_(data), indices_(indices), cindices_(cindices) {}

  T value() const;
  I index() const;
  J cindex() const;

  bool operator<(const iterator &other) const;
  bool operator>(const iterator &other) const;
  bool operator==(const iterator &other) const;
  bool operator!=(const iterator &other) const;
  bool operator<=(const iterator &other) const;
  bool operator>=(const iterator &other) const;

  iterator &operator++();
  iterator &operator--();
  iterator operator++(int);
  iterator operator--(int);

  iterator &operator+=(const difference_type &n);
  iterator &operator-=(const difference_type &n);
  iterator operator+(const difference_type &n);
  iterator operator-(const difference_type &n);
  iterator operator+(const difference_type &n) const;
  iterator operator-(const difference_type &n) const;
  difference_type operator-(const iterator &other) const;

  reference_type operator*();
  reference_type operator[](const difference_type &n);

  reference_type operator*() const;
  reference_type operator[](const difference_type &n) const;
};

template <typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
iterator<T, I, J> operator+(
    const typename iterator<T, I, J>::difference_type &n,
    iterator<T, I, J> &other) {
  iterator<T, I, J> temp(n + other.data_, n + other.indices_,
                         n + other.cindices_);
  return temp;
}

template <typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
iterator<T, I, J> operator-(
    const typename iterator<T, I, J>::difference_type &n,
    iterator<T, I, J> &other) {
  iterator<T, I, J> temp(n - other.data_, n - other.indices_,
                         n - other.cindices_);
  return temp;
}

template <typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
iterator<T, I, J> operator+(
    const typename iterator<T, I, J>::difference_type &n,
    const iterator<T, I, J> &other) {
  iterator<T, I, J> temp(n + other.data_, n + other.indices_,
                         n + other.cindices_);
  return temp;
}

template <typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
iterator<T, I, J> operator-(
    const typename iterator<T, I, J>::difference_type &n,
    const iterator<T, I, J> &other) {
  iterator<T, I, J> temp(n - other.data_, n - other.indices_,
                         n - other.cindices_);
  return temp;
}

static_assert(std::random_access_iterator<iterator<int8_t, int32_t, uint8_t>>,
              "iterator is not a random access iterator");

}  // namespace details
}  // namespace quspin
