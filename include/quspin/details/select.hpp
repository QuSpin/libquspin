#pragma once

#include <quspin/details/error.hpp>
#include <sstream>
#include <type_traits>
#include <variant>

namespace quspin {
  namespace details {

    template <typename... Types, typename Variant> std::variant<Types...> select(Variant v) {
      return visit_or_error<std::variant<Types...>>(
          [](auto &&arg) {
            using arg_t = std::decay_t<decltype(v)>;
            if constexpr ((std::is_same_v<arg_t, Types> || ...)) {
              return std::variant<Types...>(arg);
            } else {
              std::stringstream err;

              DType input_dtype = DType::of<arg_t>();
              auto expected_dtypes = std::vector<DType>{DType::of<Types>()...};
              err << "Invalid type " << input_dtype.name();
              err << ", expected one of the specified types: ";

              for (auto &dtype : expected_dtypes) {
                err << dtype.name() << ", ";
              }

              return Error(ErrorType::ValueError, err.str());
            }
          },
          v);
    }

  }  // namespace details
}  // namespace quspin
