#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <numeric>
#include <quspin/array/details/pointer.hpp>
#include <quspin/details/operators.hpp>
#include <quspin/details/type_concepts.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <variant>
#include <vector>

namespace quspin {
  namespace details {

    static constexpr std::size_t MAX_DIM = 64;

    template <PrimativeTypes T> struct array_iterator;
    template <PrimativeTypes T> struct const_array_iterator;

    template <PrimativeTypes T> struct array : public typed_object<T> {
    private:
      reference_counted_ptr<T> data_;
      std::vector<std::size_t> stride_;
      std::vector<std::size_t> shape_;
      std::size_t size_;
      std::size_t ndim_;

      std::size_t get_offset(std::vector<std::size_t> &input) const;
      void init_from_shape_(const std::vector<std::size_t> &shape);
      void init_from_buffer_(const std::vector<std::size_t> &shape,
                             const std::vector<std::size_t> &stride, T *data);

    public:
      array();
      array(const std::vector<std::size_t> &shape);
      array(const std::vector<std::size_t> &shape, const std::vector<std::size_t> &stride, T *data);
      array(std::initializer_list<T> values);

      const T *data() const;
      T *mut_data();

      const_array_iterator<T> begin() const;
      const_array_iterator<T> end() const;
      array_iterator<T> begin();
      array_iterator<T> end();

      std::size_t strides(const std::size_t &i) const;
      std::size_t shape(const std::size_t &i) const;
      std::vector<std::size_t> shape() const;
      std::vector<std::size_t> strides() const;

      std::size_t ndim() const;
      std::size_t size() const;
      std::size_t itemsize() const;
      std::size_t use_count() const;

      const T &at(std::vector<std::size_t> &input) const;
      T &at(std::vector<std::size_t> &input);

      const T &at(std::initializer_list<std::size_t> &input) const;
      T &at(std::initializer_list<std::size_t> &input);
      array<T> copy() const;
    };

    template <typename T> struct value_type<array<T>> {
      using type = T;
    };

    template <typename T> constexpr bool is_integer_array_v
        = std::is_integral_v<typename array<T>::value_type>;

    template <typename T> constexpr bool is_floating_array_v
        = std::is_floating_point_v<typename array<T>::value_type>;

    template <typename T> constexpr bool is_complex_array_v
        = std::conjunction_v<std::is_same<typename array<T>::value_type, cfloat>,
                             std::is_same<typename array<T>::value_type, cdouble>,
                             std::is_same<typename array<T>::value_type, cldouble>>;

    template <typename U, typename T> constexpr bool is_array_type_v
        = std::is_same_v<U, typename array<T>::value_type>;

    using arrays = std::variant<array<int8_t>, array<uint8_t>, array<int16_t>, array<uint16_t>,
                                array<uint32_t>, array<int32_t>, array<uint64_t>, array<int64_t>,
                                array<float>, array<double>, array<cfloat>, array<cdouble>>;

    using index_arrays = std::variant<array<int32_t>, array<int64_t>>;
    using matrix_arrays
        = std::variant<array<int8_t>, array<float>, array<double>, array<cfloat>, array<cdouble>>;
    using state_arrays = std::variant<array<double>, array<cdouble>>;

    template <typename T> constexpr bool is_index_type_v
        = std::conjunction_v<std::is_same<T, int32_t>, std::is_same<T, int64_t>>;

    template <typename T> constexpr bool is_matrix_type_v
        = std::conjunction_v<std::is_same<T, int8_t>, std::is_same<T, float>,
                             std::is_same<T, double>, std::is_same<T, cfloat>,
                             std::is_same<T, cdouble>>;

    template <typename T> constexpr bool is_state_type_v
        = std::conjunction_v<std::is_same<T, double>, std::is_same<T, cdouble>>;

  }  // namespace details
}  // namespace quspin
