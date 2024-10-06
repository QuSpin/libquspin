
#include <quspin/array/details/scalar.hpp>
#include <quspin/array/scalar.hpp>
#include <quspin/details/cast.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <quspin/dtype/dtype.hpp>

namespace quspin {

  Scalar::Scalar() { internals_ = details::scalars(details::scalar<double>(0)); }

}  // namespace quspin
