
#include <cstdint>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include <quspin/details/complex.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <quspin/dtype/dtype.hpp>

namespace quspin {

DType::DType() { internals_ = details::dtype<double>(); }
DType::DType(const details::dtypes &dtype) { this->internals_ = dtype; }

template <typename T> DType::DType(const details::dtype<T> &dtype) {
  internals_ = dtype;
}
template DType::DType(const details::dtype<int8_t> &);
template DType::DType(const details::dtype<uint8_t> &);
template DType::DType(const details::dtype<int16_t> &);
template DType::DType(const details::dtype<uint16_t> &);
template DType::DType(const details::dtype<int32_t> &);
template DType::DType(const details::dtype<uint32_t> &);
template DType::DType(const details::dtype<int64_t> &);
template DType::DType(const details::dtype<uint64_t> &);
template DType::DType(const details::dtype<float> &);
template DType::DType(const details::dtype<double> &);
template DType::DType(const details::dtype<details::cfloat> &);
template DType::DType(const details::dtype<details::cdouble> &);

DType::DType(const DType &dtype) { internals_ = dtype.internals_; }

std::string DType::name() const {

  return std::visit(
      [](const auto &dtype) {
        using T = typename std::decay_t<decltype(dtype)>::value_type;
        if constexpr (std::is_same_v<T, int8_t>) {
          return std::string("int8");
        } else if constexpr (std::is_same_v<T, uint8_t>) {
          return std::string("uint8");
        } else if constexpr (std::is_same_v<T, int16_t>) {
          return std::string("int16");
        } else if constexpr (std::is_same_v<T, uint16_t>) {
          return std::string("uint16");
        } else if constexpr (std::is_same_v<T, int32_t>) {
          return std::string("int32");
        } else if constexpr (std::is_same_v<T, uint32_t>) {
          return std::string("uint32");
        } else if constexpr (std::is_same_v<T, int64_t>) {
          return std::string("int64");
        } else if constexpr (std::is_same_v<T, uint64_t>) {
          return std::string("uint64)");
        } else if constexpr (std::is_same_v<T, float>) {
          return std::string("float");
        } else if constexpr (std::is_same_v<T, double>) {
          return std::string("double");
        } else if constexpr (std::is_same_v<T, details::cfloat>) {
          return std::string("cfloat");
        } else {
          return std::string("cdouble");
        }
      },
      internals_);
}

DType result_dtype(std::vector<DType> &args) {
  switch (args.size()) {
  case 0:
    throw std::invalid_argument("Empty list of dtypes");
  case 1:
    return args[0];
  case 2:
    return std::visit(
        [](const auto &dtype1, const auto &dtype2) {
          return DType(details::result_dtype(dtype1, dtype2));
        },
        get_variant_obj(args[0]), get_variant_obj(args[1]));
  default:
    auto next_args = std::vector<DType>(args.begin() + 1, args.end());
    DType recurse = result_dtype(next_args);

    auto new_args = std::vector<DType>({args[0], recurse});
    return result_dtype(new_args);
  }
}

bool int_dtype(const DType &dtype) {
  return std::visit(
      [](const auto &dtype) {
        using T = typename std::decay_t<decltype(dtype)>::value_type;
        return std::is_integral_v<T>;
      },
      get_variant_obj(dtype));
}

bool float_dtype(const DType &dtype) {
  return std::visit(
      [](const auto &dtype) {
        using T = typename std::decay_t<decltype(dtype)>::value_type;
        return std::is_floating_point_v<T>;
      },
      get_variant_obj(dtype));
}

bool complex_dtype(const DType &dtype) {
  return std::visit(
      [](const auto &dtype) {
        using T = typename std::decay_t<decltype(dtype)>::value_type;
        return std::is_same_v<T, details::cfloat> ||
               std::is_same_v<T, details::cdouble>;
      },
      get_variant_obj(dtype));
}

} // namespace quspin