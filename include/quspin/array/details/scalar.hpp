#pragma once
#include <quspin/details/complex.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <variant>

namespace quspin {
  namespace details {

    template <typename T> class scalar : public typed_object<T> {
      T value_;

    public:
      scalar() = default;
      scalar(T value) : value_(value) {}
      operator T() const { return value_; }
      T operator=(const T &value) {
        value_ = value;
        return value_;
      }
      T get() const { return value_; }
    };

    using scalars
        = std::variant<scalar<int8_t>, scalar<uint8_t>, scalar<int16_t>, scalar<uint16_t>,
                       scalar<uint32_t>, scalar<int32_t>, scalar<uint64_t>, scalar<int64_t>,
                       scalar<float>, scalar<double>, scalar<cfloat>, scalar<cdouble>>;

  }  // namespace details
}  // namespace quspin
