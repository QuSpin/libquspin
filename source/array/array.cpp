#include <initializer_list>
#include <iostream>
#include <numeric>
#include <quspin/array/array.hpp>
#include <quspin/array/details/array.hpp>
#include <quspin/array/reference.hpp>
#include <quspin/array/scalar.hpp>
#include <quspin/details/complex.hpp>
#include <quspin/details/error.hpp>
#include <quspin/dtype/dtype.hpp>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace quspin {

  // implementation of Array

  Array::Array() {
    internals_ = std::visit(
        [](const auto &dtype) {
          using T = typename std::decay_t<decltype(dtype)>::value_type;
          details::array<T> arr;
          return details::arrays(arr);
        },
        DType().get_variant_obj());
  }

  Array::Array(std::initializer_list<ssize_t> shape, const DType &dtype) {
    internals_ = std::visit(
        [&shape](const auto &dtype) {
          using T = typename std::decay_t<decltype(dtype)>::value_type;
          details::array<T> arr(shape);
          return details::arrays(arr);
        },
        dtype.get_variant_obj());
  }

  Array::Array(const std::vector<ssize_t> &shape, const DType &dtype) {
    internals_ = std::visit(
        [&shape](const auto &dtype) {
          using T = typename std::decay_t<decltype(dtype)>::value_type;
          details::array<T> arr(shape);
          return details::arrays(arr);
        },
        dtype.get_variant_obj());
  }

  Array::Array(const std::vector<ssize_t> &shape, const DType &dtype, void *data) {
    internals_ = std::visit(
        [&shape, &data](const auto &dtype) {
          using T = typename std::decay_t<decltype(dtype)>::value_type;
          details::array<T> arr(shape, static_cast<T *>(data));
          return details::arrays(arr);
        },
        dtype.get_variant_obj());
  }

  std::vector<ssize_t> Array::shape() const {
    return std::visit([](const auto &internals) { return internals.shape(); }, internals_);
  }

  ssize_t Array::ndim() const {
    return std::visit([](const auto &internals) { return internals.ndim(); }, internals_);
  }

  ssize_t Array::size() const {
    return std::visit([](const auto &internals) { return internals.size(); }, internals_);
  }

  const Scalar Array::operator[](std::vector<ssize_t> &index) const {
    return std::visit([&index](const auto &internals) { return Scalar(internals.at(index)); },
                      internals_);
  }

}  // namespace quspin