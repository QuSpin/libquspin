#pragma once

#include <iterator>
#include <numeric>
#include <quspin/array/details/array.hpp>
#include <quspin/details/type_concepts.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <type_traits>
#include <variant>
#include <vector>
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

    public:
      using value_type = T;
      using index_type = I;
      using cindex_type = J;

      qmatrix() = default;
      qmatrix(const std::size_t dim, array<T> &data, array<I> &indptr, array<I> &indices,
              array<J> &cindices);

      void sort();

      array<T> data() const;
      T *data_ptr();
      const T *data_ptr() const;
      T data_at(const std::size_t &i) const;

      array<I> indptr() const;
      I *indptr_ptr();
      const I *indptr_ptr() const;
      I indptr_at(const std::size_t &i) const;

      array<I> indices() const;
      I *indices_ptr();
      const I *indices_ptr() const;
      I indices_at(const std::size_t &i) const;

      array<J> cindices() const;
      J *cindices_ptr();
      const J *cindices_ptr() const;
      J cindices_at(const std::size_t &i) const;

      std::size_t dim() const;

      // iterator row_begin(const std::size_t &row);
      // iterator row_end(const std::size_t &row);
    };

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    struct value_type<qmatrix<T, I, J>> {
      using type = T;
    };

    using qmatrices
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
