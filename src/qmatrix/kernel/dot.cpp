
#include <quspin/array/array.hpp>
#include <quspin/detail/error.hpp>
#include <quspin/detail/optional.hpp>
#include <quspin/detail/select.hpp>
#include <quspin/dtype/detail/dtype.hpp>
#include <quspin/qmatrix/detail/qmatrix.hpp>
#include <quspin/qmatrix/kernel/detail/dot.hpp>
#include <quspin/qmatrix/kernel/dot.hpp>
#include <quspin/qmatrix/qmatrix.hpp>
#include <stdexcept>

namespace quspin {

Array dot(const bool overwrite_out, QMatrix op, Array coeff, Array input,
          detail::Optional<Array> output) {
  using namespace detail;

  Array output_value = output.get(
      [&input]() -> Array { return Array(input.shape(), input.dtype()); });

  auto coeff_select = select<array<double>, array<cdouble>>(coeff, "coeff");
  auto input_select = select<array<double>, array<cdouble>>(input, "input");
  auto output_select =
      select<array<double>, array<cdouble>>(output_value, "output");

  auto kernel = [overwrite_out](const auto &op, const auto &coeff,
                                const auto &input, auto &output) {
    using T = typename std::decay_t<decltype(op)>::value_type;
    using I = typename std::decay_t<decltype(op)>::index_type;
    using J = typename std::decay_t<decltype(op)>::cindex_type;
    using coeff_t = value_type_t<std::decay_t<decltype(coeff)>>;
    using in_t = value_type_t<std::decay_t<decltype(input)>>;
    using out_t = value_type_t<std::decay_t<decltype(output)>>;

    if constexpr (QMatrixDotTypes<T, I, J, coeff_t, in_t, out_t>) {
      dot(overwrite_out, op, coeff, input, output);
      return ReturnVoidError();
    } else {
      return ReturnVoidError(
          Error(ErrorType::ValueError, "Invalud imput types."));
    }
  };

  visit_or_error<std::monostate>(
      kernel, op.get_variant_obj(), coeff_select.get_variant_obj(),
      input_select.get_variant_obj(), output_select.get_variant_obj());

  return output_value;
}

}  // namespace quspin
