#pragma once

#include <complex>

namespace quspin {
  namespace details {

    template <typename T> using complex_wrapper = std::complex<T>;

    typedef std::complex<float> cfloat;
    typedef std::complex<double> cdouble;
    typedef std::complex<long double> cldouble;

    template <typename T> struct is_complex : std::false_type {};

    template <typename T> struct is_complex<std::complex<T>> : std::true_type {};

    template <typename T> inline constexpr bool is_complex_v = is_complex<T>::value;

    template <typename... Ts> using upcast_t = std::common_type_t<Ts...>;

    template <class from, class to> inline constexpr bool can_cast_v
        = std::is_convertible_v<from, to>;

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