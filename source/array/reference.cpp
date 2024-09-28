#include <type_traits>
#include <variant>

#include <quspin/array/details/reference.hpp>
#include <quspin/array/reference.hpp>
#include <quspin/array/scalar.hpp>
#include <quspin/details/cast.hpp>
#include <quspin/details/complex.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <quspin/dtype/dtype.hpp>

namespace quspin {

// implementation of Reference

Reference::Reference(Reference &reference) {
  internals_ = reference.internals_;
  dtype_ = reference.dtype_;
}

Reference::Reference(details::references &reference) {
  internals_ = reference;
  dtype_ = std::visit(
      [](auto &reference) {
        using T = typename std::decay_t<decltype(reference)>::value_type;
        return DType(details::dtype<T>());
      },
      reference);
}

template <typename T> Reference::Reference(T &ref) {
  internals_ = details::references(details::reference<T>(ref));
  dtype_ = details::dtypes(details::dtype<T>());
}
template Reference::Reference(int8_t &);
template Reference::Reference(uint8_t &);
template Reference::Reference(int16_t &);
template Reference::Reference(uint16_t &);
template Reference::Reference(int32_t &);
template Reference::Reference(uint32_t &);
template Reference::Reference(int64_t &);
template Reference::Reference(uint64_t &);
template Reference::Reference(float &);
template Reference::Reference(double &);
template Reference::Reference(details::cfloat &);
template Reference::Reference(details::cdouble &);

Reference &Reference::operator=(const Scalar &scalar) {
  std::visit(
      [](auto &internals, const auto &scalar) {
        auto scalar_value = scalar.get();

        using T = typename std::decay_t<decltype(internals)>::value_type;
        using S = std::decay_t<decltype(scalar_value)>;

        internals = details::cast<T, S>(scalar_value);
      },
      internals_, get_variant_obj(scalar));
  return *this;
}

DType Reference::dtype() const { return dtype_; }

details::references get_variant_obj(const Reference &reference) {
  return reference.internals_;
}

} // namespace quspin