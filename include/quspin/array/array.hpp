#pragma once

#include <cstddef>
#include <quspin/array/details/array.hpp>
#include <quspin/array/details/scalar.hpp>
#include <quspin/array/reference.hpp>
#include <quspin/array/scalar.hpp>
#include <quspin/details/complex.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <quspin/dtype/dtype.hpp>
#include <string>
#include <vector>

namespace quspin {

  class Array : public DTypeObject<details::arrays> {
    using DTypeObject<details::arrays>::internals_;

  public:
    Array();
    Array(std::initializer_list<ssize_t> shape, const DType &dtype);
    Array(const std::vector<ssize_t> &shape, const DType &dtype);
    Array(const std::vector<ssize_t> &shape, const DType &dtype, void *data);

    std::vector<ssize_t> shape() const;
    ssize_t ndim() const;
    ssize_t size() const;

    const Scalar operator[](std::vector<ssize_t> &index) const;
    Reference operator[](std::vector<ssize_t> &index);
  };

}  // namespace quspin