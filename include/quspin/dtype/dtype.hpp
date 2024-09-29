#pragma once

#include <string>
#include <vector>

#include <quspin/details/complex.hpp>
#include <quspin/details/variant_container.hpp>
#include <quspin/dtype/details/dtype.hpp>


namespace quspin {

class DType : public details::VariantContainer<details::dtypes> {
  using details::VariantContainer<details::dtypes>::internals_;

public:
  DType();
  DType(const details::dtypes &dtype);
  template <typename T> DType(const details::dtype<T> &dtype);
  std::string name() const;

  template <typename T> static DType of() {
    if constexpr (details::is_typed_object_v<T>) {
      return DType(details::dtype<typename T::value_type>());
    } else {
      return DType(details::dtype<T>());
    }
  }
};

DType result_dtype(std::vector<DType> &);
bool int_dtype(const DType &);
bool float_dtype(const DType &);
bool complex_dtype(const DType &);

static const DType Int8 = DType(details::dtype<int8_t>());
static const DType UInt8 = DType(details::dtype<uint8_t>());
static const DType Int16 = DType(details::dtype<int16_t>());
static const DType UInt16 = DType(details::dtype<uint16_t>());
static const DType Int32 = DType(details::dtype<int32_t>());
static const DType UInt32 = DType(details::dtype<uint32_t>());
static const DType Int64 = DType(details::dtype<int64_t>());
static const DType UInt64 = DType(details::dtype<uint64_t>());
static const DType Float = DType(details::dtype<float>());
static const DType Double = DType(details::dtype<double>());
static const DType CFloat = DType(details::dtype<details::cfloat>());
static const DType CDouble = DType(details::dtype<details::cdouble>());

template <typename Variant>
class DTypeObject : public details::VariantContainer<Variant> {
protected:
  using details::VariantContainer<Variant>::internals_;

public:
  DType dtype() const {
    return std::visit(
        [](const auto &obj) { return DType::of<decltype(obj)>(); },
        get_variant_obj(*this));
  }
};

} // namespace quspin