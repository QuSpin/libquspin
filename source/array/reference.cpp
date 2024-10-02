#include <quspin/array/details/reference.hpp>
#include <quspin/array/reference.hpp>
#include <quspin/array/scalar.hpp>
#include <quspin/details/cast.hpp>
#include <quspin/details/complex.hpp>
#include <quspin/details/variant_container.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <quspin/dtype/dtype.hpp>
#include <type_traits>
#include <variant>

namespace quspin {

  // implementation of Reference

  Reference::Reference(details::references &reference) { internals_ = reference; }

}  // namespace quspin