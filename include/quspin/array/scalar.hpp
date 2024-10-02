#pragma once

#include <quspin/array/details/scalar.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <quspin/dtype/dtype.hpp>
#include <quspin/details/cast.hpp>
#include <variant>


namespace quspin {

  class Scalar : public DTypeObject<details::scalars> {
    using DTypeObject<details::scalars>::internals_;

  public:
    Scalar();
    template <typename T> Scalar(const T &value) {
      internals_ = details::scalars(details::scalar<T>(value));
    }

    template <typename T> operator T() {
      return std::visit([](const auto &internals) { return details::cast<T>(internals.get()); },
                        internals_);
    }

    template <typename T> Scalar &operator=(const T &value);

    Scalar operator+(const Scalar &other) const;
    Scalar operator-(const Scalar &other) const;
    Scalar operator*(const Scalar &other) const;
    Scalar operator/(const Scalar &other) const;
  };

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

  template Scalar::operator int8_t();
  template Scalar::operator uint8_t();
  template Scalar::operator int16_t();
  template Scalar::operator uint16_t();
  template Scalar::operator int32_t();
  template Scalar::operator uint32_t();
  template Scalar::operator int64_t();
  template Scalar::operator uint64_t();
  template Scalar::operator float();
  template Scalar::operator double();
  template Scalar::operator details::cfloat();
  template Scalar::operator details::cdouble();

  template<typename T> Scalar &Scalar::operator=(const T &value) {
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

}  // namespace quspin