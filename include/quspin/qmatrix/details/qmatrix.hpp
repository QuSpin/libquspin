#pragma once

#include <numeric>
#include <quspin/array/details/array.hpp>
#include <quspin/details/type_concepts.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <type_traits>
#include <variant>

namespace quspin {
  namespace details {

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J> struct qmatrix
        : public typed_object<T> {
    private:
      std::size_t dim_;
      array<T> data_;
      array<I> indptr_;
      array<I> indices_;
      array<J> cindices_;
      static constexpr J max_coeff = std::numeric_limits<J>::max();

      static void check_fields(const std::size_t dim, const array<T> &data, const array<I> &indptr,
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

    public:
      using value_type = T;
      using index_type = I;
      using cindex_type = J;

      qmatrix() = default;
      qmatrix(const std::size_t dim, array<T> &data, array<I> &indptr, array<I> &indices,
              array<J> &cindices)
          : dim_(dim), data_(data), indptr_(indptr), indices_(indices), cindices_(cindices) {
        check_fields(dim, data, indptr, indices, cindices);
      }

      array<T> data() const { return data_; }
      T *data_ptr() { return data_.mut_data(); }
      const T *data_ptr() const { return data_.data(); }
      T data_at(const std::size_t &i) const { return data_ptr()[i]; }

      array<I> indptr() const { return indptr_; }
      I *indptr_ptr() { return indptr_.mut_data(); }
      const I *indptr_ptr() const { return indptr_.data(); }
      I indptr_at(const std::size_t &i) const { return indptr_ptr()[i]; }

      array<I> indices() const { return indices_; }
      I *indices_ptr() { return indices_.mut_data(); }
      const I *indices_ptr() const { return indices_.data(); }
      I indices_at(const std::size_t &i) const { return indices_ptr()[i]; }

      array<J> cindices() const { return cindices_; }
      J *cindices_ptr() { return cindices_.mut_data(); }
      const J *cindices_ptr() const { return cindices_.data(); }
      J cindices_at(const std::size_t &i) const { return cindices_ptr()[i]; }

      std::size_t dim() const { return dim_; }
    };

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    struct value_type<qmatrix<T, I, J>> {
      using type = T;
    };

    using qmatrixs
        = std::variant<qmatrix<int8_t, int32_t, uint8_t>, qmatrix<int16_t, int32_t, uint8_t>,
                       qmatrix<float, int32_t, uint8_t>, qmatrix<double, int32_t, uint8_t>,
                       qmatrix<cfloat, int32_t, uint8_t>, qmatrix<cdouble, int32_t, uint8_t>,
                       qmatrix<int8_t, int64_t, uint8_t>, qmatrix<int16_t, int64_t, uint8_t>,
                       qmatrix<float, int64_t, uint8_t>, qmatrix<double, int64_t, uint8_t>,
                       qmatrix<cfloat, int64_t, uint8_t>, qmatrix<cdouble, int64_t, uint8_t>,
                       qmatrix<int8_t, int32_t, uint16_t>, qmatrix<int16_t, int32_t, uint16_t>,
                       qmatrix<float, int32_t, uint16_t>, qmatrix<double, int32_t, uint16_t>,
                       qmatrix<cfloat, int32_t, uint16_t>, qmatrix<cdouble, int32_t, uint16_t>,
                       qmatrix<int8_t, int64_t, uint16_t>, qmatrix<int16_t, int64_t, uint16_t>,
                       qmatrix<float, int64_t, uint16_t>, qmatrix<double, int64_t, uint16_t>,
                       qmatrix<cfloat, int64_t, uint16_t>, qmatrix<cdouble, int64_t, uint16_t>>;

  }  // namespace details
}  // namespace quspin
