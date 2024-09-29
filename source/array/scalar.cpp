
#include <quspin/array/details/scalar.hpp>
#include <quspin/array/scalar.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <quspin/dtype/dtype.hpp>

namespace quspin {

  Scalar::Scalar() { internals_ = details::scalars(details::scalar<double>(0)); }

  Scalar::Scalar(const details::scalars &scalar) { internals_ = scalar; }

  template <typename T> Scalar::Scalar(const T &value) {
    internals_ = details::scalars(details::scalar<T>(value));
  }
  // explicit template instantiation
  template Scalar::Scalar(const int8_t &);
  template Scalar::Scalar(const uint8_t &);
  template Scalar::Scalar(const int16_t &);
  template Scalar::Scalar(const uint16_t &);
  template Scalar::Scalar(const int32_t &);
  template Scalar::Scalar(const uint32_t &);
  template Scalar::Scalar(const int64_t &);
  template Scalar::Scalar(const uint64_t &);
  template Scalar::Scalar(const float &);
  template Scalar::Scalar(const double &);
  template Scalar::Scalar(const details::cfloat &);
  template Scalar::Scalar(const details::cdouble &);

  Scalar &Scalar::operator=(const details::scalars &scalar) {
    internals_ = scalar;
    return *this;
  }

  template <typename T> Scalar &Scalar::operator=(const T &value) {
    internals_ = details::scalars(details::scalar<T>(value));
    return *this;
  }
  // explicit template instantiation
  template Scalar &Scalar::operator=(const int8_t &);
  template Scalar &Scalar::operator=(const uint8_t &);
  template Scalar &Scalar::operator=(const int16_t &);
  template Scalar &Scalar::operator=(const uint16_t &);
  template Scalar &Scalar::operator=(const int32_t &);
  template Scalar &Scalar::operator=(const uint32_t &);
  template Scalar &Scalar::operator=(const int64_t &);
  template Scalar &Scalar::operator=(const uint64_t &);
  template Scalar &Scalar::operator=(const float &);
  template Scalar &Scalar::operator=(const double &);
  template Scalar &Scalar::operator=(const details::cfloat &);
  template Scalar &Scalar::operator=(const details::cdouble &);

}  // namespace quspin