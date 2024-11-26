// Copyright 2024 Phillip Weinberg

#include <algorithm>
#include <iterator>
#include <quspin/array/details/array.hpp>
#include <quspin/details/type_concepts.hpp>
#include <quspin/qmatrix/details/iterator.hpp>
#include <quspin/qmatrix/details/qmatrix.hpp>
#include <ranges>
#include <type_traits>

namespace quspin { namespace details {

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
void check_fields(const std::size_t dim, const array<T> &data,
                  const array<I> &indptr, const array<I> &indices,
                  const array<J> &cindices, const std::size_t num_coeff) {
  if (num_coeff > std::numeric_limits<J>::max()) {
    throw std::invalid_argument("cindex value exceeds maximum value");
  }

  if (dim + 1 != indptr.size()) {
    throw std::invalid_argument("Invalid number of rows");
  }

  if (indices.size() != cindices.size()) {
    throw std::invalid_argument("misaligned size of indices and coefficients");
  }

  if (data.size() != indices.size()) {
    throw std::invalid_argument("misaligned size of data and indices");
  }

  if (!indptr.is_contiguous() || indptr.ndim() != 1) {
    throw std::invalid_argument("indptr must be contiguous 1D array");
  }

  if (!indices.is_contiguous() || indices.ndim() != 1) {
    throw std::invalid_argument("indices must be contiguous 1D array");
  }

  if (!cindices.is_contiguous() || cindices.ndim() != 1) {
    throw std::invalid_argument("cindices must be contiguous 1D array");
  }

  if (!data.is_contiguous() || data.ndim() != 1) {
    throw std::invalid_argument("data must be contiguous 1D array");
  }

  if (!std::is_sorted(indptr.begin(), indptr.end())) {
    throw std::invalid_argument("indptr must be sorted");
  }

  const auto nnz_ = indptr.at(std::array<std::size_t, 1>{indptr.size() - 1});

  if (nnz_ < 0) {
    throw std::invalid_argument("indptr must be positive");
  }

  const std::size_t nnz = static_cast<std::size_t>(nnz_);

  if (nnz != indices.size()) {
    throw std::invalid_argument("indptr must be sorted");
  }
  if (nnz != cindices.size()) {
    throw std::invalid_argument("indptr must be sorted");
  }
  if (nnz != data.size()) {
    throw std::invalid_argument("indptr must be sorted");
  }
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
qmatrix<T, I, J>::qmatrix(const std::size_t dim, array<T> &data,
                          array<I> &indptr, array<I> &indices,
                          array<J> &cindices)
    : dim_(dim),
      data_(data),
      indptr_(indptr),
      indices_(indices),
      cindices_(cindices) {
  num_coeff_ = static_cast<std::size_t>(
      *std::max_element(cindices.begin(), cindices.end()));
  check_fields(dim, data, indptr, indices, cindices, num_coeff_);
  if (!has_sorted_indices()) {
    sort_indices();
  }
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
qmatrix<T, I, J>::qmatrix(const std::size_t dim, array<T> &data,
                          array<I> &indptr, array<I> &indices, const J &cindex)
    : dim_(dim), data_(data), indptr_(indptr), indices_(indices) {
  cindices_ = array<J>(indices.shape());
  num_coeff_ = static_cast<std::size_t>(cindex);

  std::fill(cindices_.begin(), cindices_.end(), cindex);

  check_fields(dim, data, indptr, indices, cindices_,
               static_cast<std::size_t>(cindex));
  if (!has_sorted_indices()) {
    sort_indices();
  }
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
void qmatrix<T, I, J>::sort_indices() {
  for (auto &&i : std::ranges::iota_view{std::size_t(0), dim_}) {
    std::sort(row_begin(i), row_end(i));
  }
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
bool qmatrix<T, I, J>::has_sorted_indices() const {
  for (auto &&i : std::ranges::iota_view{std::size_t(0), dim_}) {
    if (!std::is_sorted(row_begin(i), row_end(i))) {
      return false;
    }
  }
  return true;
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
std::size_t qmatrix<T, I, J>::num_coeff() const {
  return num_coeff_;
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
array<T> qmatrix<T, I, J>::data() const {
  return data_;
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
T *qmatrix<T, I, J>::data_ptr() {
  return data_.mut_data();
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
const T *qmatrix<T, I, J>::data_ptr() const {
  return data_.data();
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
const T &qmatrix<T, I, J>::data_at(const std::size_t &i) const {
  return data_ptr()[i];
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
T &qmatrix<T, I, J>::data_at(const std::size_t &i) {
  return data_ptr()[i];
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
array<I> qmatrix<T, I, J>::indptr() const {
  return indptr_;
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
I *qmatrix<T, I, J>::indptr_ptr() {
  return indptr_.mut_data();
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
const I *qmatrix<T, I, J>::indptr_ptr() const {
  return indptr_.data();
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
const I &qmatrix<T, I, J>::indptr_at(const std::size_t &i) const {
  return indptr_ptr()[i];
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
I &qmatrix<T, I, J>::indptr_at(const std::size_t &i) {
  return indptr_ptr()[i];
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
array<I> qmatrix<T, I, J>::indices() const {
  return indices_;
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
I *qmatrix<T, I, J>::indices_ptr() {
  return indices_.mut_data();
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
const I *qmatrix<T, I, J>::indices_ptr() const {
  return indices_.data();
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
const I &qmatrix<T, I, J>::indices_at(const std::size_t &i) const {
  return indices_ptr()[i];
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
I &qmatrix<T, I, J>::indices_at(const std::size_t &i) {
  return indices_ptr()[i];
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
array<J> qmatrix<T, I, J>::cindices() const {
  return cindices_;
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
J *qmatrix<T, I, J>::cindices_ptr() {
  return cindices_.mut_data();
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
const J *qmatrix<T, I, J>::cindices_ptr() const {
  return cindices_.data();
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
const J &qmatrix<T, I, J>::cindices_at(const std::size_t &i) const {
  return cindices_ptr()[i];
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
J &qmatrix<T, I, J>::cindices_at(const std::size_t &i) {
  return cindices_ptr()[i];
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
std::size_t qmatrix<T, I, J>::dim() const {
  return dim_;
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
const iterator<T, I, J> qmatrix<T, I, J>::row_begin(
    const std::size_t &row) const {
  const std::size_t offset = indptr_at(row);
  T *data = const_cast<T *>(data_ptr() + offset);
  I *indices = const_cast<I *>(indices_ptr() + offset);
  J *cindices = const_cast<J *>(cindices_ptr() + offset);

  const iterator<T, I, J> tmp(data, indices, cindices);

  return tmp;
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
const iterator<T, I, J> qmatrix<T, I, J>::row_end(
    const std::size_t &row) const {
  const std::size_t offset = indptr_at(row + 1);
  T *data = const_cast<T *>(data_ptr() + offset);
  I *indices = const_cast<I *>(indices_ptr() + offset);
  J *cindices = const_cast<J *>(cindices_ptr() + offset);

  const iterator<T, I, J> tmp(data, indices, cindices);

  return tmp;
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
iterator<T, I, J> qmatrix<T, I, J>::row_begin(const std::size_t &row) {
  const std::size_t offset = indptr_at(row);
  return iterator<T, I, J>(data_ptr() + offset, indices_ptr() + offset,
                           cindices_ptr() + offset);
}

template<typename T, typename I, typename J>
  requires QMatrixTypes<T, I, J>
iterator<T, I, J> qmatrix<T, I, J>::row_end(const std::size_t &row) {
  const std::size_t offset = indptr_at(row + 1);
  return iterator<T, I, J>(data_ptr() + offset, indices_ptr() + offset,
                           cindices_ptr() + offset);
}

template struct qmatrix<int8_t, int32_t, uint8_t>;
template struct qmatrix<int16_t, int32_t, uint8_t>;
template struct qmatrix<float, int32_t, uint8_t>;
template struct qmatrix<double, int32_t, uint8_t>;
template struct qmatrix<cfloat, int32_t, uint8_t>;
template struct qmatrix<cdouble, int32_t, uint8_t>;
template struct qmatrix<int8_t, int64_t, uint8_t>;
template struct qmatrix<int16_t, int64_t, uint8_t>;
template struct qmatrix<float, int64_t, uint8_t>;
template struct qmatrix<double, int64_t, uint8_t>;
template struct qmatrix<cfloat, int64_t, uint8_t>;
template struct qmatrix<cdouble, int64_t, uint8_t>;
template struct qmatrix<int8_t, int32_t, uint16_t>;
template struct qmatrix<int16_t, int32_t, uint16_t>;
template struct qmatrix<float, int32_t, uint16_t>;
template struct qmatrix<double, int32_t, uint16_t>;
template struct qmatrix<cfloat, int32_t, uint16_t>;
template struct qmatrix<cdouble, int32_t, uint16_t>;
template struct qmatrix<int8_t, int64_t, uint16_t>;
template struct qmatrix<int16_t, int64_t, uint16_t>;
template struct qmatrix<float, int64_t, uint16_t>;
template struct qmatrix<double, int64_t, uint16_t>;
template struct qmatrix<cfloat, int64_t, uint16_t>;
template struct qmatrix<cdouble, int64_t, uint16_t>;

}}  // namespace quspin::details
