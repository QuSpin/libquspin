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
  concept ComplexTypes = std::same_as<std::decay_t<T>, std::complex<float>>
                         || std::same_as<std::decay_t<T>, std::complex<double>>;

  template <typename T>
  concept PrimativeTypes = RealTypes<T> || ComplexTypes<T>;

  template <typename T, typename I, typename J>
  concept QuantumOperatorTypes
      = PrimativeTypes<T> && (std::same_as<I, int32_t> || std::same_as<I, int64_t>)
        && (std::same_as<J, uint8_t> || std::same_as<J, uint16_t>);

}  // namespace quspin
