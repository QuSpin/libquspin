#include <algorithm>
#include <array>
#include <numeric>
#include <quspin/array/array.hpp>
#include <quspin/array/kernels.hpp>
#include <quspin/details/broadcast.hpp>
#include <quspin/details/cast.hpp>
#include <quspin/details/dispatch.hpp>
#include <quspin/details/error.hpp>
#include <quspin/details/math.hpp>
#include <quspin/details/operators.hpp>
#include <quspin/details/optional.hpp>
#include <type_traits>
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

  bool allclose(const Array &lhs, const Array &rhs, const double rtol /* = 1e-5 */,
                const double atol /* = 1e-8 */) {
    auto kernel = [&atol, &rtol](const auto &lhs, const auto &rhs) {
      using lhs_t = typename std::decay_t<decltype(lhs)>::value_type;
      using rhs_t = typename std::decay_t<decltype(rhs)>::value_type;
      using value_t = details::upcast_t<lhs_t, rhs_t>;

      auto check = [&atol, &rtol](const auto &lhs, const auto &rhs) {
        const value_t l = details::cast<value_t>(lhs);
        const value_t r = details::cast<value_t>(rhs);

        return details::abs(l - r) <= atol + rtol * std::max(details::abs(l), details::abs(r));
      };

      auto rhs_it = rhs.begin();
      for (auto lhs_it = lhs.begin(); lhs_it != lhs.end(); ++lhs_it, ++rhs_it) {
        if (!check(*lhs_it, *rhs_it)) {
          return details::ErrorOr<Scalar>(Scalar(static_cast<uint8_t>(false)));
        }
      }
      return details::ErrorOr<Scalar>(Scalar(static_cast<uint8_t>(true)));
    };

    auto static_args_check = [](const auto &...) { return details::ValidArgs(); };

    auto dynamic_args_check = [](const auto &lhs, const auto &rhs) {
      if (lhs.shape() != rhs.shape()) {
        return details::ReturnVoidError(
            details::Error(details::ErrorType::ValueError, "Incompatible shapes"));
      }
      return details::ReturnVoidError();
    };

    Scalar result
        = details::dispatch_scalar(kernel, static_args_check, dynamic_args_check, lhs, rhs);

    return static_cast<bool>(static_cast<uint8_t>(result));
  }

}  // namespace quspin
