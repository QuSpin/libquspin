#pragma once

#include <numeric>
#include <quspin/array/details/array.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <type_traits>
#include <variant>

namespace quspin {
  namespace details {

    template <typename T, typename I, typename J> struct qoperator : public typed_object<T> {
    private:
      std::size_t dim_;
      array<T> data_;
      array<I> indptr_;
      array<I> indices_;
      array<J> cindices_;
      static constexpr std::size_t max_coeff = std::numeric_limits<J>::max();

    public:
      qoperator() = default;

      qoperator(std::size_t dim, array<T> data, array<I> indptr, array<I> indices, array<J> cindices)
          : dim_(dim), data_(data), indptr_(indptr), indices_(indices), cindices_(cindices) {}

      T *data() { return data_.mut_data(); }
      const T *data() const { return data_.data(); }

      I *indptr() { return indptr_.mut_data(); }
      const I *indptr() const { return indptr_.data(); }

      I *indices() { return indices_.mut_data(); }
      const I *indices() const { return indices_.data(); }

      J *cindices() { return cindices_.mut_data(); }
      const J *cindices() const { return cindices_.data(); }

      std::size_t dim() const { return dim_; }
    };

    using qoperators
        = std::variant<qoperator<int8_t, int32_t, uint8_t>, qoperator<int16_t, int32_t, uint8_t>,
                       qoperator<float, int32_t, uint8_t>, qoperator<double, int32_t, uint8_t>,
                       qoperator<cfloat, int32_t, uint8_t>, qoperator<cdouble, int32_t, uint8_t>,
                       qoperator<int8_t, int64_t, uint8_t>, qoperator<int16_t, int64_t, uint8_t>,
                       qoperator<float, int64_t, uint8_t>, qoperator<double, int64_t, uint8_t>,
                       qoperator<cfloat, int64_t, uint8_t>, qoperator<cdouble, int64_t, uint8_t>,
                       qoperator<int8_t, int32_t, uint16_t>, qoperator<int16_t, int32_t, uint16_t>,
                       qoperator<float, int32_t, uint16_t>, qoperator<double, int32_t, uint16_t>,
                       qoperator<cfloat, int32_t, uint16_t>, qoperator<cdouble, int32_t, uint16_t>,
                       qoperator<int8_t, int64_t, uint16_t>, qoperator<int16_t, int64_t, uint16_t>,
                       qoperator<float, int64_t, uint16_t>, qoperator<double, int64_t, uint16_t>,
                       qoperator<cfloat, int64_t, uint16_t>, qoperator<cdouble, int64_t, uint16_t>>;

  }  // namespace details
}  // namespace quspin