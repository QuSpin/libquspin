
#include <algorithm>
#include <quspin/array/details/array.hpp>
#include <quspin/details/type_concepts.hpp>
#include <quspin/qmatrix/details/qmatrix.hpp>
#include <type_traits>

namespace quspin {
  namespace details {

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    void check_fields(const std::size_t dim, const array<T> &data, const array<I> &indptr,
                      const array<I> &indices, const array<J> &cindices) {
      if (dim != indptr.size() - 1) {
        throw std::runtime_error("Invalid number of rows");
      }
      if (indices.size() != cindices.size()) {
        throw std::runtime_error("misaligned size of indices and coefficients");
      }
      if (data.size() != indices.size()) {
        throw std::runtime_error("misaligned size of data and indices");
      }
      if (!indptr.is_contiguous() || indptr.ndim() != 1) {
        throw std::runtime_error("indptr must be contiguous 1D array");
      }
      if (!indices.is_contiguous() || indices.ndim() != 1) {
        throw std::runtime_error("indices must be contiguous 1D array");
      }
      if (!cindices.is_contiguous() || cindices.ndim() != 1) {
        throw std::runtime_error("cindices must be contiguous 1D array");
      }
      if (!data.is_contiguous() || data.ndim() != 1) {
        throw std::runtime_error("data must be contiguous 1D array");
      }
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    qmatrix<T, I, J>::qmatrix(const std::size_t dim, array<T> &data, array<I> &indptr,
                              array<I> &indices, array<J> &cindices)
        : dim_(dim), data_(data), indptr_(indptr), indices_(indices), cindices_(cindices) {
      check_fields(dim, data, indptr, indices, cindices);
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    void qmatrix<T, I, J>::sort() {
      for (std::size_t i = 0; i < dim_; ++i) {
        auto r_begin = row_begin(i);
        auto r_end = row_end(i);
        std::sort(r_begin, r_end);
      }
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    array<T> qmatrix<T, I, J>::data() const {
      return data_;
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    T *qmatrix<T, I, J>::data_ptr() {
      return data_.mut_data();
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    const T *qmatrix<T, I, J>::data_ptr() const {
      return data_.data();
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    const T &qmatrix<T, I, J>::data_at(const std::size_t &i) const {
      return data_ptr()[i];
    }
    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    T &qmatrix<T, I, J>::data_at(const std::size_t &i) {
      return data_ptr()[i];
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    array<I> qmatrix<T, I, J>::indptr() const {
      return indptr_;
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    I *qmatrix<T, I, J>::indptr_ptr() {
      return indptr_.mut_data();
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    const I *qmatrix<T, I, J>::indptr_ptr() const {
      return indptr_.data();
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    const I &qmatrix<T, I, J>::indptr_at(const std::size_t &i) const {
      return indptr_ptr()[i];
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    I &qmatrix<T, I, J>::indptr_at(const std::size_t &i) {
      return indptr_ptr()[i];
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    array<I> qmatrix<T, I, J>::indices() const {
      return indices_;
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    I *qmatrix<T, I, J>::indices_ptr() {
      return indices_.mut_data();
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    const I *qmatrix<T, I, J>::indices_ptr() const {
      return indices_.data();
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    const I &qmatrix<T, I, J>::indices_at(const std::size_t &i) const {
      return indices_ptr()[i];
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    I &qmatrix<T, I, J>::indices_at(const std::size_t &i) {
      return indices_ptr()[i];
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    array<J> qmatrix<T, I, J>::cindices() const {
      return cindices_;
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    J *qmatrix<T, I, J>::cindices_ptr() {
      return cindices_.mut_data();
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    const J *qmatrix<T, I, J>::cindices_ptr() const {
      return cindices_.data();
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    const J &qmatrix<T, I, J>::cindices_at(const std::size_t &i) const {
      return cindices_ptr()[i];
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    J &qmatrix<T, I, J>::cindices_at(const std::size_t &i) {
      return cindices_ptr()[i];
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    std::size_t qmatrix<T, I, J>::dim() const {
      return dim_;
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    iterator<const T, const I, const J> qmatrix<T, I, J>::row_begin(const std::size_t &row) const {
      const std::size_t offset = indptr_at(row);
      return iterator<const T, const I, const J>(data_ptr() + offset, indices_ptr() + offset,
                                                 cindices_ptr() + offset);
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    iterator<const T, const I, const J> qmatrix<T, I, J>::row_end(const std::size_t &row) const {
      const std::size_t offset = indptr_at(row + 1);
      return iterator<const T, const I, const J>(data_ptr() + offset, indices_ptr() + offset,
                                                 cindices_ptr() + offset);
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
    iterator<T, I, J> qmatrix<T, I, J>::row_begin(const std::size_t &row) {
      const std::size_t offset = indptr_at(row);
      return iterator<T, I, J>(data_ptr() + offset, indices_ptr() + offset,
                               cindices_ptr() + offset);
    }

    template <typename T, typename I, typename J>
      requires QuantumOperatorTypes<T, I, J>
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

  }  // namespace details
}  // namespace quspin
