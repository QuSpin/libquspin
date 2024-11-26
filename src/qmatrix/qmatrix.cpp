// Copyright 2024 Phillip Weinberg

#include <quspin/array/array.hpp>
#include <quspin/array/detail/array.hpp>
#include <quspin/detail/select.hpp>
#include <quspin/qmatrix/detail/qmatrix.hpp>
#include <quspin/qmatrix/qmatrix.hpp>
#include <type_traits>
#include <variant>

namespace quspin {

decltype(auto) select_data_variants(Array &data) {
  using namespace detail;
  return select<array<int8_t>, array<int16_t>, array<float>, array<double>,
                array<cfloat>, array<cdouble>>(data, "data");
}

decltype(auto) select_indptr_variants(Array &indptr) {
  using namespace detail;
  return select<array<int32_t>, array<int64_t>>(indptr, "indptr");
}

decltype(auto) select_indices_variants(Array &indptr) {
  using namespace detail;
  return select<array<int32_t>, array<int64_t>>(indptr, "indices");
}

decltype(auto) select_cindex_variants(Array &cindices) {
  using namespace detail;
  return select<array<uint8_t>, array<uint16_t>>(cindices, "cindices");
}

QMatrix::QMatrix(Array data, Array indptr, Array indices, Array cindices) {
  using namespace detail;

  const std::size_t dim = indptr.size() - 1;

  auto data_select = select_data_variants(data);
  auto indptr_select = select_indptr_variants(indptr);
  auto indices_select = select_indices_variants(indices);
  auto cindices_select = select_cindex_variants(cindices);

  auto constructor = [&dim](auto &&data, auto &&indptr, auto &&indices,
                            auto &&cindices) {
    if constexpr (!std::is_same_v<decltype(indptr), decltype(indices)>) {
      return ErrorOr<qmatrices>(
          Error(ErrorType::ValueError,
                "indptr and indices must have the same dtype"));
    } else {
      try {
        qmatrices op = qmatrix(dim, data, indptr, indices, cindices);
        return ErrorOr<qmatrices>(op);
      } catch (const std::invalid_argument &e) {
        return ErrorOr<qmatrices>(Error(ErrorType::ValueError, e.what()));
      } catch (...) {
        return ErrorOr<qmatrices>(
            Error(ErrorType::RuntimeError, "Unknown error"));
      }
    }
  };

  DTypeObject<detail::qmatrices>::internals_ = visit_or_error<qmatrices>(
      constructor, data_select.get_variant_obj(),
      indptr_select.get_variant_obj(), indices_select.get_variant_obj(),
      cindices_select.get_variant_obj());
}

template<typename J>
  requires std::same_as<J, uint8_t> || std::same_as<J, uint16_t>
QMatrix::QMatrix(Array data, Array indptr, Array indices, const J cindex) {
  using namespace detail;

  const std::size_t dim = indptr.size() - 1;

  auto data_select = select_data_variants(data);
  auto indptr_select = select_indptr_variants(indptr);
  auto indices_select = select_indices_variants(indices);

  auto constructor = [&dim, &cindex](auto &&data, auto &&indptr,
                                     auto &&indices) {
    if constexpr (!std::is_same_v<decltype(indptr), decltype(indices)>) {
      return ErrorOr<qmatrices>(
          Error(ErrorType::ValueError,
                "indptr and indices must have the same dtype"));
    } else {
      try {
        qmatrices op = qmatrix(dim, data, indptr, indices, cindex);
        return ErrorOr<qmatrices>(op);
      } catch (const std::invalid_argument &e) {
        return ErrorOr<qmatrices>(Error(ErrorType::ValueError, e.what()));
      } catch (...) {
        return ErrorOr<qmatrices>(
            Error(ErrorType::RuntimeError, "Unknown error"));
      }
    }
  };

  DTypeObject<detail::qmatrices>::internals_ = visit_or_error<qmatrices>(
      constructor, data_select.get_variant_obj(),
      indptr_select.get_variant_obj(), indices_select.get_variant_obj());
}

template QMatrix::QMatrix(Array data, Array indptr, Array indices,
                          const uint8_t cindex);
template QMatrix::QMatrix(Array data, Array indptr, Array indices,
                          const uint16_t cindex);

std::size_t QMatrix::dim() const {
  return std::visit([](auto &&arg) { return arg.dim(); },
                    DTypeObject<detail::qmatrices>::internals_);
}

Array QMatrix::indptr() const {
  return std::visit([](auto &&arg) { return Array(arg.indptr()); },
                    DTypeObject<detail::qmatrices>::internals_);
}

Array QMatrix::indices() const {
  return std::visit([](auto &&arg) { return Array(arg.indices()); },
                    DTypeObject<detail::qmatrices>::internals_);
}

Array QMatrix::data() const {
  return std::visit([](auto &&arg) { return Array(arg.data()); },
                    DTypeObject<detail::qmatrices>::internals_);
}

Array QMatrix::cindices() const {
  return std::visit([](auto &&arg) { return Array(arg.cindices()); },
                    DTypeObject<detail::qmatrices>::internals_);
}

}  // namespace quspin
