#pragma once
#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4100)
#endif

#include <cassert>
#include <quspin/array/array.hpp>
#include <quspin/array/details/array.hpp>
#include <quspin/array/scalar.hpp>
#include <quspin/details/error.hpp>
#include <quspin/details/operators.hpp>
#include <quspin/details/optional.hpp>

namespace quspin {
  namespace details {

    struct ValidArgs {
      static constexpr bool has_error() { return false; }
    };

    template <typename Kernel, typename StaticArgsCheck, typename DynamicArgsCheck,
              typename... Args>
    void dispatch_core(Kernel &&kernel, StaticArgsCheck &&static_args_check,
                       DynamicArgsCheck &&dynamic_args_check, Array out, Args... args) {
      ReturnVoidError err = dynamic_args_check(out, args...);

      if (err.has_error()) {
        err.get_error().throw_error();
      }

      visit_or_error<std::monostate>(
          [&kernel](auto out, const auto &...args) {
            using can_dispatch_t = decltype(static_args_check(out, args...));

            if constexpr (can_dispatch_t::has_error()) {
              return ReturnVoidError(can_dispatch_t::get_error());
            } else {
              return kernel(out, args...);
            }
          },
          out.get_variant_obj(), args.get_variant_obj()...);
    }

    template <typename Kernel, typename ResultShape, typename ResultDType, typename StaticArgsCheck,
              class DynamicArgsCheck, typename... Args>
    Array dispatch_general(Kernel &&kernel, ResultShape &&result_shape_func,
                           ResultDType &&result_dtype_func, StaticArgsCheck &&static_args_check,
                           DynamicArgsCheck &&dynamic_args_check, Optional<Array> out_option,
                           Args... args) {
      std::vector<std::size_t> shape = result_shape_func(args...);
      DType output_dtype = result_dtype_func(args...);

      auto default_result = [&shape, &output_dtype]() { return Array(shape, output_dtype); };

      Array out = out_option.get(default_result);

      dispatch_core(kernel, static_args_check, dynamic_args_check, out, args...);

      return out;
    }

    template <typename Kernel, typename... Args>
    Array dispatch_elementwise(Kernel &&kernel, Optional<Array> out, const Array &lhs,
                               const Array &rhs) {
      struct InvalidDtype {
        static constexpr bool has_error() { return true; }
        static Error get_error() { return Error(ErrorType::ValueError, "Incompatible out type"); }
      };

      auto result_shape = [](const Array &lhs, const Array &rhs) {
        assert(lhs.shape() == rhs.shape());
        return lhs.shape();
      };

      auto result_dtype_func = [](const Array &lhs, const Array &rhs) {
        std::vector<DType> dtypes = {lhs.dtype(), rhs.dtype()};
        return result_dtype(dtypes);
      };

      auto static_args_check = [](auto &out, const auto &lhs, const auto &rhs) {
        using lhs_t = typename std::decay_t<decltype(lhs)>::value_type;
        using rhs_t = typename std::decay_t<decltype(rhs)>::value_type;
        using out_t = typename std::decay_t<decltype(out)>::value_type;
        using result_t = std::common_type_t<lhs_t, rhs_t>;

        if constexpr (std::is_convertible_v<result_t, out_t>) {
          return ValidArgs();
        } else {
          return InvalidDtype();
        }
      };

      auto dynamic_args_check = [](auto &out, const auto &lhs, const auto &rhs) {
        if (lhs.shape() != rhs.shape() || lhs.shape() != out.shape()) {
          return ReturnVoidError(Error(ErrorType::ValueError, "Incompatible shapes"));
        }
        return ReturnVoidError();
      };

      return dispatch_general(kernel, result_shape, result_dtype_func, static_args_check,
                              dynamic_args_check, out, lhs, rhs);
    }

    template <typename Kernel, typename StaticArgsCheck, typename DynamicArgsCheck,
              typename... Args>
    Scalar dispatch_scalar(Kernel &&kernel, StaticArgsCheck &&static_args_check,
                           DynamicArgsCheck &&dynamic_args_check, const Args &...args) {
      ReturnVoidError err = dynamic_args_check(args...);

      if (err.has_error()) {
        err.get_error().throw_error();
      }

      return visit_or_error<Scalar>(
          [&kernel](const auto &...args) {
            using can_dispatch_t = decltype(static_args_check(args...));

            if constexpr (can_dispatch_t::has_error()) {
              return ErrorOr<Scalar>(can_dispatch_t::get_error());
            } else {
              return kernel(args...);
            }
          },
          args.get_variant_obj()...);
    }

  }  // namespace details
}  // namespace quspin

#ifdef _MSC_VER
#  pragma warning(pop)
#endif