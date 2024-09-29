#include <array>
#include <quspin/array/array.hpp>
#include <quspin/details/broadcast.hpp>
#include <quspin/details/complex.hpp>
#include <quspin/details/dispatch.hpp>
#include <quspin/details/error.hpp>
#include <quspin/details/math.hpp>
#include <quspin/details/optional.hpp>
#include <quspin/kernel/kernels.hpp>

namespace quspin {

  using OptionalArray = details::Optional<Array>;

  Array add(const Array &lhs, const Array &rhs,
            details::Optional<Array> out /* = details::Optional<Array>() */) {
    auto kernel = [](auto &out, const auto &lhs, const auto &rhs) {
      using out_t = typename std::decay_t<decltype(out)>::value_type;
      using lhs_t = typename std::decay_t<decltype(lhs)>::value_type;
      using rhs_t = typename std::decay_t<decltype(rhs)>::value_type;
      using result_t = std::common_type_t<lhs_t, rhs_t>;

      std::transform(
          lhs.cbegin(), lhs.cend(), rhs.cbegin(), out.begin(),
          [](const auto &lhs, const auto &rhs) {
            return static_cast<out_t>(static_cast<result_t>(lhs) + static_cast<result_t>(rhs));
          });
      return details::ReturnVoidError();
    };
    return details::dispatch_elementwise(kernel, out, lhs, rhs);
  }

  Scalar norm(const Array &arr) {
    auto kernel = [](const auto &arr) {
      using T = typename std::decay_t<decltype(arr)>::value_type;
      using norm_t = std::decay_t<decltype(details::abs_squared(T(0)))>;

      constexpr ssize_t batch = 64 / sizeof(norm_t);

      std::array<norm_t, batch> norms({norm_t(0)});

      ssize_t niter = arr.size() / norms.size();
      ssize_t nleft = arr.size() % norms.size();

      auto ptr = arr.cbegin();

      for (ssize_t i = 0; i < niter; i++) {
        for (auto &norm : norms) {
          norm += details::abs_squared(*ptr++);
        }
      }
      norm_t extra = norm_t(0);
      for (ssize_t i = 0; i < nleft; i++) {
        extra += details::abs_squared(*ptr++);
      }
      Scalar result = std::accumulate(norms.cbegin(), norms.cend(), extra);
      return details::ErrorOr<Scalar>(result);
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
