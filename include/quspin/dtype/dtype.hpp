#pragma once

#include <quspin/details/operators.hpp>
#include <quspin/details/variant_container.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <string>
#include <vector>

namespace quspin {

  class DType : public details::VariantContainer<details::dtypes> {
    using details::VariantContainer<details::dtypes>::internals_;

    static details::dtypes default_value() { return details::dtypes(details::dtype<double>()); }

  public:
    DType() : details::VariantContainer<details::dtypes>(default_value()) {};
    template <typename T> DType(const details::dtype<T> &dtype) {
      internals_ = details::dtypes(dtype);
    }
    std::string name() const;

    template <typename T> static DType of() {
      using val_t = details::value_type_t<std::decay_t<T>>;
      return DType(details::dtype<details::value_type_t<val_t>>());
    }

    bool operator==(const DType &dtype) const;
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

  template <typename Variant> class DTypeObject : public details::VariantContainer<Variant> {
  protected:
    using details::VariantContainer<Variant>::internals_;

  public:
    DTypeObject() = default;
    DTypeObject(const DTypeObject &obj) = default;
    DTypeObject(DTypeObject &&obj) = default;
    DTypeObject(const Variant &internals) : details::VariantContainer<Variant>(internals) {}
    DType dtype() const {
      return std::visit([](const auto &obj) { return DType::of<decltype(obj)>(); }, internals_);
    }
  };

}  // namespace quspin
