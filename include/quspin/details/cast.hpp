#pragma once

namespace quspin {
  namespace details {

    template <typename T> struct is_complex : std::false_type {};

    template <typename T> struct is_complex<std::complex<T>> : std::true_type {};

    template <typename T> inline constexpr bool is_complex_v = is_complex<T>::value;

    template <typename U, typename T> inline U cast(const T &value) {
      if constexpr (is_complex_v<T> && !is_complex_v<U>) {
        const auto real = value.real();
        if constexpr (std::is_convertible_v<decltype(real), U>) {
          return static_cast<U>(real);
        } else {
          return U(real);
        }
      } else {
        if constexpr (std::is_convertible_v<T, U>) {
          return static_cast<U>(value);
        } else {
          return U(value);
        }
      }
    }
  }  // namespace details
}  // namespace quspin