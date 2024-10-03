#pragma once

#include <functional>
#include <quspin/array/details/scalar.hpp>
#include <quspin/details/cast.hpp>
#include <quspin/details/operators.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <quspin/dtype/dtype.hpp>
#include <variant>

namespace quspin {

  class Scalar : public DTypeObject<details::scalars> {
    using DTypeObject<details::scalars>::internals_;

  public:
    Scalar();
    template <typename T> Scalar(const T &value);

    template <typename T> operator T() const;

    template <typename T> Scalar &operator=(const T &value);

    template <typename Op> static Scalar binary_op(const Scalar &lhs, const Scalar &rhs, Op &&op);
    template <typename T> Scalar operator+(const T &other) const;
    template <typename T> Scalar operator-(const T &other) const;
    template <typename T> Scalar operator*(const T &other) const;
    template <typename T> Scalar operator/(const T &other) const;
  };

  template <typename T> Scalar::Scalar(const T &value) {
    internals_ = details::scalars(details::scalar<T>(value));
  }
  template Scalar::Scalar(const int8_t &);
  template Scalar::Scalar(const uint8_t &);
  template Scalar::Scalar(const int16_t &);
  template Scalar::Scalar(const uint16_t &);
  template Scalar::Scalar(const int32_t &);
  template Scalar::Scalar(const uint32_t &);
  template Scalar::Scalar(const int64_t &);
  template Scalar::Scalar(const uint64_t &);
  template Scalar::Scalar(const float &);
  template Scalar::Scalar(const double &);
  template Scalar::Scalar(const details::cfloat &);
  template Scalar::Scalar(const details::cdouble &);

  template <typename T> Scalar::operator T() const {
    return std::visit([](const auto &internals) { return details::cast<T>(internals.get()); },
                      internals_);
  }
  template Scalar::operator int8_t() const;
  template Scalar::operator uint8_t() const;
  template Scalar::operator int16_t() const;
  template Scalar::operator uint16_t() const;
  template Scalar::operator int32_t() const;
  template Scalar::operator uint32_t() const;
  template Scalar::operator int64_t() const;
  template Scalar::operator uint64_t() const;
  template Scalar::operator float() const;
  template Scalar::operator double() const;
  template Scalar::operator details::cfloat() const;
  template Scalar::operator details::cdouble() const;

  template <typename T> Scalar &Scalar::operator=(const T &value) {
    internals_ = details::scalars(details::scalar<T>(value));
    return *this;
  }

  template Scalar &Scalar::operator=(const int8_t &);
  template Scalar &Scalar::operator=(const uint8_t &);
  template Scalar &Scalar::operator=(const int16_t &);
  template Scalar &Scalar::operator=(const uint16_t &);
  template Scalar &Scalar::operator=(const int32_t &);
  template Scalar &Scalar::operator=(const uint32_t &);
  template Scalar &Scalar::operator=(const int64_t &);
  template Scalar &Scalar::operator=(const uint64_t &);
  template Scalar &Scalar::operator=(const float &);
  template Scalar &Scalar::operator=(const double &);
  template Scalar &Scalar::operator=(const details::cfloat &);
  template Scalar &Scalar::operator=(const details::cdouble &);

  template <typename Op> Scalar Scalar::binary_op(const Scalar &lhs, const Scalar &rhs, Op &&op) {
    return std::visit([&op](auto &&lhs, auto &&rhs) { return Scalar(op(lhs.get(), rhs.get())); },
                      lhs.get_variant_obj(), rhs.get_variant_obj());
  }

  template <typename T> Scalar Scalar::operator+(const T &other) const {
    return Scalar::binary_op(*this, Scalar(other),
                             [](auto &&lhs, auto &&rhs) { return lhs - rhs; });
  }
  template Scalar Scalar::operator+(const int8_t &) const;
  template Scalar Scalar::operator+(const uint8_t &) const;
  template Scalar Scalar::operator+(const int16_t &) const;
  template Scalar Scalar::operator+(const uint16_t &) const;
  template Scalar Scalar::operator+(const int32_t &) const;
  template Scalar Scalar::operator+(const uint32_t &) const;
  template Scalar Scalar::operator+(const int64_t &) const;
  template Scalar Scalar::operator+(const uint64_t &) const;
  template Scalar Scalar::operator+(const float &) const;
  template Scalar Scalar::operator+(const double &) const;
  template Scalar Scalar::operator+(const details::cfloat &) const;
  template Scalar Scalar::operator+(const details::cdouble &) const;
  template Scalar Scalar::operator+(const Scalar &) const;

  template <typename T> Scalar Scalar::operator-(const T &other) const {
    return Scalar::binary_op(*this, Scalar(other),
                             [](auto &&lhs, auto &&rhs) { return lhs - rhs; });
  }
  template Scalar Scalar::operator-(const int8_t &) const;
  template Scalar Scalar::operator-(const uint8_t &) const;
  template Scalar Scalar::operator-(const int16_t &) const;
  template Scalar Scalar::operator-(const uint16_t &) const;
  template Scalar Scalar::operator-(const int32_t &) const;
  template Scalar Scalar::operator-(const uint32_t &) const;
  template Scalar Scalar::operator-(const int64_t &) const;
  template Scalar Scalar::operator-(const uint64_t &) const;
  template Scalar Scalar::operator-(const float &) const;
  template Scalar Scalar::operator-(const double &) const;
  template Scalar Scalar::operator-(const details::cfloat &) const;
  template Scalar Scalar::operator-(const details::cdouble &) const;
  template Scalar Scalar::operator-(const Scalar &) const;

  template <typename T> Scalar Scalar::operator*(const T &other) const {
    return Scalar::binary_op(*this, Scalar(other),
                             [](auto &&lhs, auto &&rhs) { return lhs * rhs; });
  }
  template Scalar Scalar::operator*(const int8_t &) const;
  template Scalar Scalar::operator*(const uint8_t &) const;
  template Scalar Scalar::operator*(const int16_t &) const;
  template Scalar Scalar::operator*(const uint16_t &) const;
  template Scalar Scalar::operator*(const int32_t &) const;
  template Scalar Scalar::operator*(const uint32_t &) const;
  template Scalar Scalar::operator*(const int64_t &) const;
  template Scalar Scalar::operator*(const uint64_t &) const;
  template Scalar Scalar::operator*(const float &) const;
  template Scalar Scalar::operator*(const double &) const;
  template Scalar Scalar::operator*(const details::cfloat &) const;
  template Scalar Scalar::operator*(const details::cdouble &) const;
  template Scalar Scalar::operator*(const Scalar &) const;

  template <typename T> Scalar Scalar::operator/(const T &other) const {
    return Scalar::binary_op(*this, Scalar(other),
                             [](auto &&lhs, auto &&rhs) { return lhs / rhs; });
  }
  template Scalar Scalar::operator/(const int8_t &) const;
  template Scalar Scalar::operator/(const uint8_t &) const;
  template Scalar Scalar::operator/(const int16_t &) const;
  template Scalar Scalar::operator/(const uint16_t &) const;
  template Scalar Scalar::operator/(const int32_t &) const;
  template Scalar Scalar::operator/(const uint32_t &) const;
  template Scalar Scalar::operator/(const int64_t &) const;
  template Scalar Scalar::operator/(const uint64_t &) const;
  template Scalar Scalar::operator/(const float &) const;
  template Scalar Scalar::operator/(const double &) const;
  template Scalar Scalar::operator/(const details::cfloat &) const;
  template Scalar Scalar::operator/(const details::cdouble &) const;

}  // namespace quspin