#pragma once

#include <numeric>
#include <quspin/array/details/array.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <type_traits>
#include <variant>

namespace quspin {
  namespace details {

    template <typename T, typename I, typename J> struct quantum_operator : public typed_object<T> {
    private:
      std::size_t dim_;
      array<T> data_;
      array<I> indptr_;
      array<I> indices_;
      array<J> cindices_;
      static constexpr J max_coeff = std::numeric_limits<J>::max();

    public:
      quantum_operator() = default;

      quantum_operator(std::size_t dim, array<T> &data, array<I> &indptr, array<I> &indices,
                       array<J> &cindices)
          : dim_(dim), data_(data), indptr_(indptr), indices_(indices), cindices_(cindices) {}

      T *data() { return data_.mut_data(); }
      const T *data() const { return data_.data(); }
      T data(const std::size_t &i) const { return data()[i]; }

      I *indptr() { return indptr_.mut_data(); }
      const I *indptr() const { return indptr_.data(); }
      I indptr(const std::size_t &i) const { return indptr()[i]; }

      I *indices() { return indices_.mut_data(); }
      const I *indices() const { return indices_.data(); }
      I indices(const std::size_t &i) const { return indices()[i]; }

      J *cindices() { return cindices_.mut_data(); }
      const J *cindices() const { return cindices_.data(); }
      J cindices(const std::size_t &i) const { return cindices()[i]; }

      std::size_t dim() const { return dim_; }
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