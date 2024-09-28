#pragma once

#include <quspin/array/details/scalar.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <quspin/dtype/dtype.hpp>

namespace quspin {

class Scalar {
private:
  details::scalars internals_;
  DType dtype_;

  friend details::scalars get_variant_obj(const Scalar &);

public:
  Scalar();
  Scalar(const Scalar &scalar);
  Scalar(const details::scalars &scalar);
  template <typename T> Scalar(const T &value);

  Scalar &operator=(const Scalar &scalar);
  Scalar &operator=(const details::scalars &scalar);
  template <typename T> Scalar &operator=(const T &value);

  DType dtype() const;
};

details::scalars get_variant_obj(const Scalar &);

} // namespace quspin