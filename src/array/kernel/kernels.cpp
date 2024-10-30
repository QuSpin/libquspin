#include <algorithm>
#include <array>
#include <numeric>
#include <quspin/array/array.hpp>
#include <quspin/array/kernel/kernels.hpp>
#include <quspin/details/broadcast.hpp>
#include <quspin/details/cast.hpp>
#include <quspin/details/dispatch.hpp>
#include <quspin/details/error.hpp>
#include <quspin/details/math.hpp>
#include <quspin/details/operators.hpp>
#include <quspin/details/optional.hpp>
#include <variant>

namespace quspin {

  using OptionalArray = details::Optional<Array>;

  Array add(const Array &lhs, const Array &rhs,
            details::Optional<Array> out /* = details::Optional<Array>() */) {
    auto kernel = [](auto &out, const auto &lhs, const auto &rhs) {
      using out_t = typename std::decay_t<decltype(out)>::value_type;

      std::transform(
          lhs.begin(), lhs.end(), rhs.begin(), out.begin(),
          [](const auto &lhs, const auto &rhs) { return details::cast<out_t>(lhs + rhs); });
      return details::ReturnVoidError();
    };
    return details::dispatch_elementwise(kernel, out, lhs, rhs);
  }

  Scalar norm(const Array &arr) {
    auto kernel = [](const auto &arr) {
      using T = typename std::decay_t<decltype(arr)>::value_type;
      using norm_t = std::decay_t<decltype(details::abs_squared(T()))>;

      norm_t result = std::accumulate(arr.begin(), arr.end(), norm_t(), [](auto &&acc, auto &&val) {
        return acc + details::abs_squared(val);
      });

      return details::ErrorOr<Scalar>(Scalar(details::cast<double>(std::sqrt(result))));
    };

    auto static_args_check = [](const auto &) { return details::ValidArgs(); };

    auto dynamic_args_check = [](const auto &arr) {
      if (arr.ndim() != 1) {
        return details::ReturnVoidError(
            details::Error(details::ErrorType::ValueError, "Expecting 1-dimensional array"));
      }
      return details::ReturnVoidError();
    };

    return details::dispatch_scalar(kernel, static_args_check, dynamic_args_check, arr);
  }

}  // namespace quspin
