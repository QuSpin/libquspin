#pragma once

namespace quspin {
  namespace details {

    template <typename T> struct is_complex : std::false_type {};

    template <typename T> struct is_complex<std::complex<T>> : std::true_type {};

    template <typename T> inline constexpr bool is_complex_v = is_complex<T>::value;

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4244)
#endif

    template <typename To, typename From> inline To cast(const From &value) {
      if constexpr (is_complex_v<From> && !is_complex_v<To>) {
        return static_cast<To>(value.real());
      } else {
        return static_cast<To>(value);
      }
    }
#ifdef _MSC_VER
#  pragma warning(pop)
#endif

  }  // namespace details
}  // namespace quspin