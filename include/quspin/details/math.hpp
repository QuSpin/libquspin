#pragma once

#include <cmath>
#include <complex>

#include <quspin/details/complex.hpp>

namespace quspin {
namespace details {

template <typename T> T abs(const complex_wrapper<T> &A) {
  return std::hypot(A.real(), A.imag());
}

template <typename T> T abs(const T &A) { return std::abs(A); }

template <typename T>
double abs_squared(const complex_wrapper<T> &A) { // abs(A)^2
  const T real = A.real();
  const T imag = A.imag();
  return real * real + imag * imag;
}

template <typename T>
T abs_squared(const T &A) requires std::is_floating_point_v<T>
{
  return A * A;
}

template <typename T> // cast integers to double for abs_squared
double abs_squared(const T &A) requires std::is_integral_v<T>
{
  return double(A * A);
}

template <typename T> inline T conj(const T &A) { return A; }

template <typename T>
T sqrt(const T &A) requires std::is_floating_point_v<T>
{
  return std::sqrt(A);
}

} // namespace details
} // namespace quspin