

#include <type_traits>
#include <variant>

#include <quspin/array/array.hpp>
#include <quspin/array/details/array.hpp>

#include <quspin/qoperator/details/qoperator.hpp>
#include <quspin/qoperator/qoperator.hpp>

#include <quspin/details/select.hpp>

namespace quspin {

Operator::Operator(Array data, Array indptr, Array indices, Array cindices) {

  using namespace details;

  const ssize_t dim = indptr.size() - 1;

  auto data_variants =
      select<array<int8_t>, array<int16_t>, array<float>, array<double>,
             array<cfloat>, array<cdouble>>(get_variant_obj(data));
  auto indptr_variants = select<array<int32_t>, array<int64_t>>(get_variant_obj(indptr));
  auto indices_variants = select<array<uint8_t>, array<uint16_t>>(get_variant_obj(indices));
  auto cindices_variants = select<array<uint8_t>, array<uint16_t>>(get_variant_obj(cindices));

  this->internals_ = visit_or_error<details::qoperators>(
      [&dim](const auto &data, const auto &indptr, const auto &indices,
             const auto &cindices) {
        if constexpr (!std::is_same_v<decltype(indptr), decltype(indices)>) {
          return Error(ErrorType::ValueError,
                       "indptr and indices must have the same dtype");
        } else {
          return operators(qoperator{dim, data, indptr, indices, cindices});
        }
      },
      data_variants, indptr_variants, indices_variants, cindices_variants);
}

} // namespace quspin
