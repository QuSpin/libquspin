#pragma once

#include <quspin/details/error.hpp>
#include <sstream>
#include <type_traits>
#include <variant>

namespace quspin {
  namespace details {

    template <typename... Types, typename Variant> std::variant<Types...> select(Variant variant) {
      using select_variant_t = std::variant<Types...>;
      return visit_or_error<std::variant<Types...>>(
          [](auto &&arg) {
            using arg_t = std::decay_t<decltype(arg)>;
            if constexpr ((std::is_same_v<arg_t, Types> || ...)) {
              select_variant_t select_variant = static_cast<select_variant_t>(arg);
              return ErrorOr<select_variant_t>(select_variant);
            } else {
              std::stringstream error_msg;

              DType input_dtype = DType::of<arg_t>();
              auto expected_dtypes = std::vector<DType>{DType::of<Types>()...};
              error_msg << "Invalid type " << input_dtype.name();
              error_msg << ", expected one of the specified types: ";

              for (auto &dtype : expected_dtypes) {
                error_msg << dtype.name() << ", ";
              }
              Error error = Error(ErrorType::ValueError, error_msg.str());
              return ErrorOr<select_variant_t>(error);
            }
          },
          variant);
    }

  }  // namespace details
}  // namespace quspin
