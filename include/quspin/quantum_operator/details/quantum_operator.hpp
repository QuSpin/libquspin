#pragma once

#include <numeric>
#include <quspin/array/details/array.hpp>
#include <quspin/details/type_concepts.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <type_traits>
#include <variant>

namespace quspin {
  namespace details {

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J> struct quantum_operator
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

      quantum_operator() = default;

      quantum_operator(std::size_t dim, array<T> &data, array<I> &indptr, array<I> &indices,
                       array<J> &cindices)
          : dim_(dim), data_(data), indptr_(indptr), indices_(indices), cindices_(cindices) {}

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
    struct value_type<quantum_operator<T, I, J>> {
      using type = T;
    };

    using quantum_operators = std::variant<
        quantum_operator<int8_t, int32_t, uint8_t>, quantum_operator<int16_t, int32_t, uint8_t>,
        quantum_operator<float, int32_t, uint8_t>, quantum_operator<double, int32_t, uint8_t>,
        quantum_operator<cfloat, int32_t, uint8_t>, quantum_operator<cdouble, int32_t, uint8_t>,
        quantum_operator<int8_t, int64_t, uint8_t>, quantum_operator<int16_t, int64_t, uint8_t>,
        quantum_operator<float, int64_t, uint8_t>, quantum_operator<double, int64_t, uint8_t>,
        quantum_operator<cfloat, int64_t, uint8_t>, quantum_operator<cdouble, int64_t, uint8_t>,
        quantum_operator<int8_t, int32_t, uint16_t>, quantum_operator<int16_t, int32_t, uint16_t>,
        quantum_operator<float, int32_t, uint16_t>, quantum_operator<double, int32_t, uint16_t>,
        quantum_operator<cfloat, int32_t, uint16_t>, quantum_operator<cdouble, int32_t, uint16_t>,
        quantum_operator<int8_t, int64_t, uint16_t>, quantum_operator<int16_t, int64_t, uint16_t>,
        quantum_operator<float, int64_t, uint16_t>, quantum_operator<double, int64_t, uint16_t>,
        quantum_operator<cfloat, int64_t, uint16_t>, quantum_operator<cdouble, int64_t, uint16_t>>;

  }  // namespace details
}  // namespace quspin
