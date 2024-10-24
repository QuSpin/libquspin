#pragma once

#include <quspin/details/error.hpp>
#include <sstream>
#include <type_traits>
#include <variant>

namespace quspin {
  namespace details {

    template <typename... Types, typename Variants>
    DTypeObject<std::variant<Types...>> select(DTypeObject<Variants> obj) {
      using select_variant_t = DTypeObject<std::variant<Types...>>;
      return visit_or_error<select_variant_t>(
          [](auto &&arg) {
            using arg_t = std::decay_t<decltype(arg)>;
            if constexpr (std::is_convertible_v<arg_t, select_variant_t>) {
              select_variant_t select_variant(arg);
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
          obj.get_variant_obj());
    }

  }  // namespace details
}  // namespace quspin
