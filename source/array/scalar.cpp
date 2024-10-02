
#include <quspin/array/details/scalar.hpp>
#include <quspin/array/scalar.hpp>
#include <quspin/details/cast.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <quspin/dtype/dtype.hpp>

namespace quspin {

  Scalar::Scalar() { internals_ = details::scalars(details::scalar<double>(0)); }

  template <typename Op> Scalar binary_op(const Scalar &lhs, const Scalar &rhs, Op &&op) {
    return std::visit([&op](auto &&lhs, auto &&rhs) { return Scalar(op(lhs.get(), rhs.get())); },
                      lhs.get_variant_obj(), rhs.get_variant_obj());
  }

  Scalar Scalar::operator+(const Scalar &other) const {
    auto op = [](auto &&lhs, auto &&rhs) { return lhs + rhs; };
    return binary_op(*this, other, op);
  }
  Scalar Scalar::operator-(const Scalar &other) const {
    auto op = [](auto &&lhs, auto &&rhs) { return lhs - rhs; };
    return binary_op(*this, other, op);
  }
  Scalar Scalar::operator*(const Scalar &other) const {
    auto op = [](auto &&lhs, auto &&rhs) { return lhs * rhs; };
    return binary_op(*this, other, op);
  }
  Scalar Scalar::operator/(const Scalar &other) const {
    auto op = [](auto &&lhs, auto &&rhs) { return lhs / rhs; };
    return binary_op(*this, other, op);
  }
}  // namespace quspin