#pragma once

#include <cstdint>
#include <quspin/details/complex.hpp>
#include <variant>

namespace quspin {
  namespace details {

    template <typename T> struct typed_object {
    public:
      using value_type = T;
    };

    template <typename T> struct dtype : public typed_object<T> {
      dtype() = default;
      static T default_value() { return T(); }
    };

    template <typename... Ts> dtype<std::common_type_t<Ts...>> result_dtype(dtype<Ts>...) {
      return dtype<std::common_type_t<Ts...>>();
    }

    template <typename... Ts> std::common_type<Ts...> object_result_type(typed_object<Ts>...) {
      return std::common_type<Ts...>();
    }

    template <typename T> struct is_typed_object : std::false_type {};

    template <typename T> struct is_typed_object<typed_object<T>> : std::true_type {};

    template <typename T> constexpr bool is_typed_object_v = is_typed_object<T>::value;

    using dtypes = std::variant<dtype<int8_t>, dtype<uint8_t>, dtype<int16_t>, dtype<uint16_t>,
                                dtype<uint32_t>, dtype<int32_t>, dtype<uint64_t>, dtype<int64_t>,
                                dtype<float>, dtype<double>, dtype<cfloat>, dtype<cdouble>>;

  }  // namespace details
}  // namespace quspin