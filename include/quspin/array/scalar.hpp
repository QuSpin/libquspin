#pragma once

#include <functional>
#include <quspin/array/details/scalar.hpp>
#include <quspin/details/cast.hpp>
#include <quspin/details/operators.hpp>
#include <quspin/details/type_concepts.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <quspin/dtype/dtype.hpp>
#include <variant>

namespace quspin {

  class Scalar : public DTypeObject<details::scalars> {
    using DTypeObject<details::scalars>::internals_;

    details::scalars default_value() { return details::scalars(details::scalar<double>()); }

  public:
    Scalar() : DTypeObject<details::scalars>(default_value()) {}
    Scalar(const Scalar &scalar) = default;
    template <PrimativeTypes T> Scalar(const T &value)
        : DTypeObject<details::scalars>(details::scalars(details::scalar(value))) {}

    template <PrimativeTypes T> operator T() const;

    template <PrimativeTypes T> Scalar &operator=(const T &value);

    template <typename Op> static Scalar binary_op(const Scalar &lhs, const Scalar &rhs, Op &&op);
    template <PrimativeTypes T> Scalar operator+(const T &other) const;
    Scalar operator+(const Scalar &other) const;
    template <PrimativeTypes T> Scalar operator-(const T &other) const;
    Scalar operator-(const Scalar &other) const;
    template <PrimativeTypes T> Scalar operator*(const T &other) const;
    Scalar operator*(const Scalar &other) const;
    template <PrimativeTypes T> Scalar operator/(const T &other) const;
    Scalar operator/(const Scalar &other) const;
  };

  template <class T>
  concept ScalarTypes = PrimativeTypes<T> || std::same_as<T, Scalar>;

}  // namespace quspin
