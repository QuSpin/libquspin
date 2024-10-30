#pragma once

#include <cstddef>
#include <quspin/array/details/array.hpp>
#include <quspin/array/details/scalar.hpp>
#include <quspin/array/reference.hpp>
#include <quspin/array/scalar.hpp>
#include <quspin/details/operators.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <quspin/dtype/dtype.hpp>
#include <string>
#include <vector>

namespace quspin {

  class Array : public DTypeObject<details::arrays> {
    using DTypeObject<details::arrays>::internals_;

    static details::arrays default_value() { return details::arrays(details::array<double>()); }

  public:
    Array() : DTypeObject<details::arrays>(default_value()) {}
    Array(const std::vector<std::size_t> &shape, const std::vector<std::size_t> &stride,
          const DType &dtype, void *data);
    Array(const std::vector<std::size_t> &shape, const DType &dtype = DType());
    Array(std::initializer_list<std::size_t> shape, std::initializer_list<std::size_t> stride,
          const DType &dtype, void *data)
        : Array(std::vector<std::size_t>(shape), std::vector<std::size_t>(stride), dtype, data) {}
    Array(std::initializer_list<std::size_t> shape, const DType &dtype = DType())
        : Array(std::vector<std::size_t>(shape), dtype) {}
    template <PrimativeTypes T> Array(const details::array<T> &array)
        : DTypeObject<details::arrays>(details::arrays(array)) {}
    template <PrimativeTypes T> Array(std::initializer_list<T> values)
        : DTypeObject<details::arrays>(details::arrays(details::array<T>(values))) {}
    std::vector<std::size_t> shape() const;
    std::size_t shape(const std::size_t &) const;
    std::vector<std::size_t> strides() const;
    std::size_t strides(const std::size_t &) const;
    std::size_t ndim() const;
    std::size_t size() const;

    const Scalar operator[](std::vector<std::size_t> &index) const;
    Reference operator[](std::vector<std::size_t> &index);
    const Scalar operator[](std::initializer_list<std::size_t> index) const;
    Reference operator[](std::initializer_list<std::size_t> index);
    Array copy() const;
    Array astype(const DType &) const;
  };
}  // namespace quspin
