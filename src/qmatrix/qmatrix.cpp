

#include <quspin/array/array.hpp>
#include <quspin/array/details/array.hpp>
#include <quspin/details/select.hpp>
#include <quspin/qmatrix/details/qmatrix.hpp>
#include <quspin/qmatrix/qmatrix.hpp>
#include <type_traits>
#include <variant>

namespace quspin {

  decltype(auto) select_data_variants(const Array &data) {
    using namespace details;
    return select<array<int8_t>, array<int16_t>, array<float>, array<double>, array<cfloat>,
                  array<cdouble>>(data, "data");
  }

  decltype(auto) select_indptr_variants(const Array &indptr) {
    using namespace details;
    return select<array<int32_t>, array<int64_t>>(indptr, "indptr");
  }

  decltype(auto) select_indices_variants(const Array &indptr) {
    using namespace details;
    return select<array<int32_t>, array<int64_t>>(indptr, "indices");
  }

  decltype(auto) select_cindex_variants(const Array &cindices) {
    using namespace details;
    return select<array<uint8_t>, array<uint16_t>>(cindices, "cindices");
  }

  QuantumOperator::QuantumOperator(Array data, Array indptr, Array indices, Array cindices) {
    using namespace details;

    const std::size_t dim = indptr.size() - 1;

    auto data_select = select_data_variants(data);
    auto indptr_select = select_indptr_variants(indptr);
    auto indices_select = select_indices_variants(indices);
    auto cindices_select = select_cindex_variants(cindices);

    auto constructor = [&dim](auto &&data, auto &&indptr, auto &&indices, auto &&cindices) {
      if constexpr (!std::is_same_v<decltype(indptr), decltype(indices)>) {
        return ErrorOr<qmatrixs>(
            Error(ErrorType::ValueError, "indptr and indices must have the same dtype"));
      } else {
        qmatrixs op = qmatrix(dim, data, indptr, indices, cindices);
        return ErrorOr<qmatrixs>(op);
      }
    };

    DTypeObject<details::qmatrixs>::internals_ = visit_or_error<qmatrixs>(
        constructor, data_select.get_variant_obj(), indptr_select.get_variant_obj(),
        indices_select.get_variant_obj(), cindices_select.get_variant_obj());
  }

  std::size_t QuantumOperator::dim() const {
    return std::visit([](auto &&arg) { return arg.dim(); },
                      DTypeObject<details::qmatrixs>::internals_);
  }

  Array QuantumOperator::indptr() const {
    return std::visit([](auto &&arg) { return Array(arg.indptr()); },
                      DTypeObject<details::qmatrixs>::internals_);
  }

  Array QuantumOperator::indices() const {
    return std::visit([](auto &&arg) { return Array(arg.indices()); },
                      DTypeObject<details::qmatrixs>::internals_);
  }

  Array QuantumOperator::data() const {
    return std::visit([](auto &&arg) { return Array(arg.data()); },
                      DTypeObject<details::qmatrixs>::internals_);
  }

  Array QuantumOperator::cindices() const {
    return std::visit([](auto &&arg) { return Array(arg.cindices()); },
                      DTypeObject<details::qmatrixs>::internals_);
  }

}  // namespace quspin