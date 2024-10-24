#include <initializer_list>
#include <iostream>
#include <numeric>
#include <quspin/array/array.hpp>
#include <quspin/array/details/array.hpp>
#include <quspin/array/reference.hpp>
#include <quspin/array/scalar.hpp>
#include <quspin/details/error.hpp>
#include <quspin/details/operators.hpp>
#include <quspin/dtype/dtype.hpp>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace quspin {

  // implementation of Array

  template Array::Array(const details::array<int8_t> &);
  template Array::Array(const details::array<uint8_t> &);
  template Array::Array(const details::array<int16_t> &);
  template Array::Array(const details::array<uint16_t> &);
  template Array::Array(const details::array<uint32_t> &);
  template Array::Array(const details::array<int32_t> &);
  template Array::Array(const details::array<uint64_t> &);
  template Array::Array(const details::array<int64_t> &);
  template Array::Array(const details::array<float> &);
  template Array::Array(const details::array<double> &);
  template Array::Array(const details::array<details::cfloat> &);
  template Array::Array(const details::array<details::cdouble> &);

  Array::Array(std::initializer_list<std::size_t> shape, const DType &dtype) {
    DTypeObject<details::arrays>::internals_ = std::visit(
        [&shape](const auto &d) {
          using T = typename std::decay_t<decltype(d)>::value_type;
          details::array<T> arr(shape);
          return details::arrays(arr);
        },
        dtype.get_variant_obj());
  }

  Array::Array(const std::vector<std::size_t> &shape, const DType &dtype) {
    DTypeObject<details::arrays>::internals_ = std::visit(
        [&shape](const auto &dtype) {
          using T = typename std::decay_t<decltype(dtype)>::value_type;
          details::array<T> arr(shape);
          return details::arrays(arr);
        },
        dtype.get_variant_obj());
  }

  Array::Array(const std::vector<std::size_t> &shape, const std::vector<std::size_t> &stride,
               const DType &dtype, void *data) {
    DTypeObject<details::arrays>::internals_ = std::visit(
        [&shape, &stride, &data](const auto &dtype) {
          using T = typename std::decay_t<decltype(dtype)>::value_type;
          details::array<T> arr(shape, stride, static_cast<T *>(data));
          return details::arrays(arr);
        },
        dtype.get_variant_obj());
  }

  Array::Array(std::initializer_list<std::size_t> shape, std::initializer_list<std::size_t> stride,
               const DType &dtype, void *data) {
    DTypeObject<details::arrays>::internals_ = std::visit(
        [&shape, &stride, &data](const auto &dtype) {
          using T = typename std::decay_t<decltype(dtype)>::value_type;
          details::array<T> arr(shape, stride, static_cast<T *>(data));
          return details::arrays(arr);
        },
        dtype.get_variant_obj());
  }

  std::vector<std::size_t> Array::shape() const {
    return std::visit([](const auto &internals) { return internals.shape(); }, internals_);
  }

  std::size_t Array::shape(const std::size_t &dim) const {
    return std::visit([dim](const auto &internals) { return internals.shape(dim); }, internals_);
  }

  std::vector<std::size_t> Array::strides() const {
    return std::visit([](const auto &internals) { return internals.strides(); }, internals_);
  }

  std::size_t Array::strides(const std::size_t &dim) const {
    return std::visit([dim](const auto &internals) { return internals.strides(dim); }, internals_);
  }

  std::size_t Array::ndim() const {
    return std::visit([](const auto &internals) { return internals.ndim(); }, internals_);
  }

  std::size_t Array::size() const {
    return std::visit([](const auto &internals) { return internals.size(); }, internals_);
  }

  const Scalar Array::operator[](std::vector<std::size_t> &index) const {
    return std::visit([&index](const auto &internals) { return Scalar(internals.at(index)); },
                      internals_);
  }

  Reference Array::operator[](std::vector<std::size_t> &index) {
    return std::visit([&index](auto &&internals) { return Reference(internals.at(index)); },
                      internals_);
  }

  const Scalar Array::operator[](std::initializer_list<std::size_t> indices) const {
    auto index = std::vector<std::size_t>(indices);
    return std::visit([&index](const auto &internals) { return Scalar(internals.at(index)); },
                      internals_);
  }

  Reference Array::operator[](std::initializer_list<std::size_t> indices) {
    auto index = std::vector<std::size_t>(indices);
    return std::visit([&index](auto &&internals) { return Reference(internals.at(index)); },
                      internals_);
  }

  Array Array::copy() const {
    return std::visit([](const auto &internals) { return Array(internals.copy()); }, internals_);
  }

}  // namespace quspin
