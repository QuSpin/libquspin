#pragma once

#include <variant>

#include <quspin/array/details/reference.hpp>
#include <quspin/array/scalar.hpp>

#include <quspin/dtype/details/dtype.hpp>
#include <quspin/dtype/dtype.hpp>

namespace quspin {

class Reference : public DTypeObject<details::references> {
  using DTypeObject<details::references>::internals_;

public:
  Reference();
  Reference(details::references &reference);
  template <typename T> Reference(T &ref);

  operator Scalar() const {
    return std::visit([](auto &internals) { return Scalar(internals.get()); },
                      internals_);
  }
  Reference &operator=(const Scalar &scalar);

  DType dtype() const;
};

details::references get_variant_obj(Reference &);

} // namespace quspin