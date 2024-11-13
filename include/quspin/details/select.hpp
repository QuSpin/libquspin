#pragma once

#include <quspin/details/error.hpp>
#include <sstream>
#include <type_traits>
#include <variant>

namespace quspin {
  namespace details {

    template <typename... Types, typename Variants>
    DTypeObject<std::variant<Types...>> select(DTypeObject<Variants> &obj, std::string name = "") {
      using select_variant_t = std::variant<Types...>;
      using dtype_object_t = DTypeObject<select_variant_t>;
      return visit_or_error<dtype_object_t>(
          [&name](auto &&arg) -> ErrorOr<dtype_object_t> {
            using arg_t = std::remove_reference_t<decltype(arg)>;
            static_assert(!std::is_const_v<arg_t>,
                          "Cannot select from const object, use a non-const object");

            if constexpr ((std::is_same_v<arg_t, Types> || ...)) {
              select_variant_t select_variant(arg);
              dtype_object_t select_obj(select_variant);
              return ErrorOr<dtype_object_t>(select_obj);
            } else {
              std::stringstream error_msg;

              DType input_dtype = DType::of<arg_t>();
              auto expected_dtypes = std::vector<DType>{DType::of<Types>()...};
              error_msg << "Invalid type " << input_dtype.name();
              if (!name.empty()) {
                error_msg << " for " << name;
              }
              error_msg << ", expected one of the specified types: ";

              for (auto &dtype : expected_dtypes) {
                error_msg << dtype.name() << ", ";
              }
              Error error = Error(ErrorType::ValueError, error_msg.str());
              return ErrorOr<dtype_object_t>(error);
            }
          },
          obj.get_variant_obj());
    }

  }  // namespace details
}  // namespace quspin
