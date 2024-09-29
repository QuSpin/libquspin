#pragma once

namespace quspin {
  namespace details {

    template <typename T> struct is_complex : std::false_type {};

    template <typename T> struct is_complex<std::complex<T>> : std::true_type {};

    template <typename T> inline constexpr bool is_complex_v = is_complex<T>::value;

    template <typename U, typename T> inline U cast(const T &value) {
      if constexpr (is_complex_v<T> && !is_complex_v<U>) {
        return static_cast<U>(value.real());
      } else {
        return static_cast<U>(value);
      }
    }

  }  // namespace details
}  // namespace quspin