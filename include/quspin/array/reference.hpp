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

  public:
    Reference();
    Reference(details::references &reference);
    template <typename T> Reference(T &ref) {
      internals_ = details::references(details::reference<T>(ref));
    }
    template <typename T> operator T() const {
      return std::visit([](auto &&internals) { return details::cast<T>(internals.get()); },
                        internals_);
    }
    template <typename T> Reference &operator=(const T &scalar) {
      if constexpr (std::is_same_v<T, Scalar>) {
        std::visit(
            [](auto &&internals, auto &&scalar) {
              using ref_t = typename std::decay_t<decltype(internals)>::value_type;
              internals = details::cast<ref_t>(scalar.get());
            },
            internals_, scalar.get_variant_obj());
      } else {
        std::visit(
            [&scalar](auto &&internals) {
              using ref_t = typename std::decay_t<decltype(internals)>::value_type;
              internals = details::cast<ref_t>(scalar);
            },
            internals_);
      }
      return *this;
    }

    template <typename T> Scalar &operator+(const T &other) const;
    template <typename T> Scalar &operator-(const T &other) const;
    template <typename T> Scalar &operator*(const T &other) const;
    template <typename T> Scalar &operator/(const T &other) const;
    bool operator==(const Scalar &other) const;
    bool operator!=(const Scalar &other) const;
  };

  template Reference::Reference(int8_t &);
  template Reference::Reference(int16_t &);
  template Reference::Reference(int32_t &);
  template Reference::Reference(int64_t &);
  template Reference::Reference(uint8_t &);
  template Reference::Reference(uint16_t &);
  template Reference::Reference(uint32_t &);
  template Reference::Reference(uint64_t &);
  template Reference::Reference(float &);
  template Reference::Reference(double &);
  template Reference::Reference(details::cfloat &);
  template Reference::Reference(details::cdouble &);

  template Reference::operator int8_t() const;
  template Reference::operator int16_t() const;
  template Reference::operator int32_t() const;
  template Reference::operator int64_t() const;
  template Reference::operator uint8_t() const;
  template Reference::operator uint16_t() const;
  template Reference::operator uint32_t() const;
  template Reference::operator uint64_t() const;
  template Reference::operator float() const;
  template Reference::operator double() const;
  template Reference::operator details::cfloat() const;
  template Reference::operator details::cdouble() const;
  template Reference::operator Scalar() const;

  template Reference &Reference::operator=(const int8_t &);
  template Reference &Reference::operator=(const int16_t &);
  template Reference &Reference::operator=(const int32_t &);
  template Reference &Reference::operator=(const int64_t &);
  template Reference &Reference::operator=(const uint8_t &);
  template Reference &Reference::operator=(const uint16_t &);
  template Reference &Reference::operator=(const uint32_t &);
  template Reference &Reference::operator=(const uint64_t &);
  template Reference &Reference::operator=(const float &);
  template Reference &Reference::operator=(const double &);
  template Reference &Reference::operator=(const details::cfloat &);
  template Reference &Reference::operator=(const details::cdouble &);
  template Reference &Reference::operator=(const Scalar &);

}  // namespace quspin