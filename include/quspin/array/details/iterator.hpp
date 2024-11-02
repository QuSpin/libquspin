#pragma once

#include <array>
#include <quspin/array/details/array.hpp>
#include <quspin/details/type_concepts.hpp>

namespace quspin {
  namespace details {

    template <PrimativeTypes T> struct array_iterator {
    private:
      array<T> &parent;
      std::size_t index;
      const std::size_t ndim;
      std::array<std::size_t, 3 * MAX_DIM> step_size_index;

    public:
      array_iterator(array<T> &arr, std::size_t start);
      bool operator==(array_iterator<T> &other) const;
      array_iterator<T> &operator++();
      T &operator*();
    };

    template <PrimativeTypes T> struct const_array_iterator {
    private:
      const array<T> &parent;
      std::size_t index;
      const std::size_t ndim;
      std::array<std::size_t, 3 * MAX_DIM> step_size_index;

    public:
      const_array_iterator(const array<T> &arr, std::size_t start);
      bool operator==(const_array_iterator<T> &other) const;
      const_array_iterator<T> &operator++();
      const T &operator*();
    };

  }  // namespace details
}  // namespace quspin
