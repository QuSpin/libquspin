#pragma once

#include <cmath>
#include <complex>
#include <quspin/details/cast.hpp>
#include <quspin/details/operators.hpp>

namespace quspin {
  namespace details {

    template <typename T> T abs(const complex_wrapper<T> &A) {
      return std::hypot(A.real(), A.imag());
    }

    template <typename T> T abs(const T &A) { return std::abs(A); }

    template <typename T> double abs_squared(const complex_wrapper<T> &A) {  // abs(A)^2
      const T real = A.real();
      const T imag = A.imag();
      return real * real + imag * imag;
    }

    template <typename T> T abs_squared(const T &A) {
      if constexpr (std::is_integral_v<T>) {
        return cast<double>(A * A);
      } else {
        return A * A;
      }
    }

  }  // namespace details
}  // namespace quspin
