

#include <quspin/array/array.hpp>
#include <quspin/array/details/array.hpp>
#include <quspin/details/select.hpp>
#include <quspin/qoperator/details/qoperator.hpp>
#include <quspin/qoperator/qoperator.hpp>
#include <type_traits>
#include <variant>

namespace quspin {

  decltype(auto) select_data_variants(const Array &data) {
    using namespace details;

    return select<array<int8_t>, array<int16_t>, array<float>, array<double>,
                  array<cfloat>, array<cdouble>>(data.get_variant_obj());
  }
  
  decltype(auto) select_index_variants(const Array &indptr) {
    using namespace details;

    return select<array<int32_t>, array<int64_t>>(indptr.get_variant_obj());
  }

  decltype(auto) select_cindex_variants(const Array &cindices) {
    using namespace details;

    return select<array<uint8_t>, array<uint16_t>>(cindices.get_variant_obj());
  }

  Operator::Operator(Array data, Array indptr, Array indices, Array cindices) {
    using namespace details;

    const std::size_t dim = indptr.size() - 1;

    auto data_variants = select_data_variants(data);
    auto indptr_variants = select_index_variants(indptr);
    auto indices_variants = select_index_variants(indices);
    auto cindices_variants = select_cindex_variants(cindices);

    auto constructor = [&dim](auto &&data, auto &&indptr, auto &&indices, auto &&cindices) {
      if constexpr (!std::is_same_v<decltype(indptr), decltype(indices)>) {
        return ErrorOr<qoperators>(Error(ErrorType::ValueError, "indptr and indices must have the same dtype"));
      } else {
        qoperators op = qoperator(dim, data, indptr, indices, cindices);
        return ErrorOr<qoperators>(op);
      }
    };

    this->internals_ = visit_or_error<qoperators>(
        constructor,
        data_variants, indptr_variants, indices_variants, cindices_variants);
  }

}  // namespace quspin
