#pragma once

#include <complex>
#include <concepts>
#include <type_traits>

namespace quspin {
  template <typename T>
  concept Floating = std::floating_point<T>;

  template <typename T>
  concept Integral = std::integral<T>;

  template <typename T>
  concept RealTypes = std::integral<T> || std::floating_point<T>;

  template <typename T>
  concept ComplexTypes
      = std::same_as<T, std::complex<float>> || std::same_as<T, std::complex<double>>;

  template <typename T>
  concept PrimativeTypes = RealTypes<T> || ComplexTypes<T>;

}  // namespace quspin
