#pragma once

#include <array>
#include <quspin/details/type_concepts.hpp>
#include <vector>

namespace quspin {
  namespace details {

    template <PrimativeTypes T> struct array_iterator {
    private:
      static constexpr std::size_t MAX_DIM = 64;
      T *data_;
      std::size_t index_;
      std::size_t ndim_;
      std::array<std::size_t, 3 * MAX_DIM> step_size_index_;

    public:
      array_iterator(T *data, const std::vector<std::size_t> &shape,
                     const std::vector<std::size_t> &strides, std::size_t start);
      array_iterator(array_iterator<T> &other) = default;
      array_iterator(array_iterator<T> &&other) = default;
      array_iterator &operator=(array_iterator<T> &other) = default;
      array_iterator &operator=(array_iterator<T> &&other) = default;

      bool operator==(array_iterator<T> &other) const;
      array_iterator<T> &operator++();
      T &operator*();
    };

    template <PrimativeTypes T> struct const_array_iterator {
    private:
      static constexpr std::size_t MAX_DIM = 64;
      const T *data_;
      std::size_t index_;
      std::size_t ndim_;
      std::array<std::size_t, 3 * MAX_DIM> step_size_index_;

    public:
      const_array_iterator(const T *data, const std::vector<std::size_t> &shape,
                           const std::vector<std::size_t> &strides, std::size_t start);
      const_array_iterator(const_array_iterator<T> &other) = default;
      const_array_iterator(const_array_iterator<T> &&other) = default;
      const_array_iterator &operator=(const_array_iterator<T> &other) = default;
      const_array_iterator &operator=(const_array_iterator<T> &&other) = default;

      bool operator==(const_array_iterator<T> &other);
      const_array_iterator<T> &operator++();
      const T &operator*();
    };

  }  // namespace details
}  // namespace quspin
