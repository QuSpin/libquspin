// Copyright 2024 Phillip Weinberg
#pragma once

#include <quspin/array/details/reference.hpp>
#include <quspin/array/scalar.hpp>
#include <quspin/details/cast.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <quspin/dtype/dtype.hpp>
#include <variant>

namespace quspin {

class Reference : public DTypeObject<details::references> {
  using DTypeObject<details::references>::internals_;

  details::references default_value() {
    return details::references(details::reference<double>());
  }

 public:
  Reference() : DTypeObject<details::references>(default_value()) {}
  Reference(details::references &reference)
      : DTypeObject<details::references>(reference) {}
  template <PrimativeTypes T>
  Reference(T &ref)
      : DTypeObject<details::references>(
            details::references(details::reference<T>(ref))) {}
  template <ScalarTypes T>
  operator T() const {
    if constexpr (std::is_same_v<T, Scalar>) {
      return std::visit(
          [](auto &&internals) { return Scalar(internals.get()); }, internals_);
    } else {
      return std::visit(
          [](auto &&internals) { return details::cast<T>(internals.get()); },
          internals_);
    }
  }
  template <ScalarTypes T>
  Reference &operator=(const T &scalar) {
    if constexpr (std::is_same_v<T, Scalar>) {
      std::visit(
          [](auto &&internals, auto &&scalar) {
            using ref_t =
                typename std::decay_t<decltype(internals)>::value_type;
            internals = details::cast<ref_t>(scalar.get());
          },
          internals_, scalar.get_variant_obj());
    } else {
      std::visit(
          [&scalar](auto &&internals) {
            using ref_t =
                typename std::decay_t<decltype(internals)>::value_type;
            internals = details::cast<ref_t>(scalar);
          },
          internals_);
    }
    return *this;
  }

  template <typename Op>
  static Scalar binary_op(const Reference &lhs, const Scalar &rhs, Op &&op);
  template <ScalarTypes T>
  Scalar operator+(const T &other) const;
  Scalar operator+(const Reference &other) const;
  template <ScalarTypes T>
  Scalar operator-(const T &other) const;
  Scalar operator-(const Reference &other) const;
  template <ScalarTypes T>
  Scalar operator*(const T &other) const;
  Scalar operator*(const Reference &other) const;
  template <ScalarTypes T>
  Scalar operator/(const T &other) const;
  Scalar operator/(const Reference &other) const;
  bool operator==(const Scalar &other) const;
  bool operator!=(const Scalar &other) const;
};

}  // namespace quspin
