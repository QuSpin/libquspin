#pragma once

#include <quspin/details/type_concepts.hpp>

namespace quspin {
  namespace details {

    template <PrimativeTypes T> struct array;

    template <PrimativeTypes T> struct array_iterator {
    private:
      static constexpr std::size_t MAX_DIM = array<T>::MAX_DIM;
      array<T> &parent;
      std::size_t index;
      const std::size_t ndim;

      std::array<std::size_t, 3 * MAX_DIM> step_size_index;

    public:
      array_iterator(array<T> &arr, std::size_t start)
          : parent(arr), index(start), ndim(arr.ndim()) {
        step_size_index.fill(std::size_t());

        for (std::size_t dim = 0; dim < arr.ndim(); dim++) {
          const std::size_t step = arr.strides(dim) / sizeof(T);
          const std::size_t dim_size = arr.shape(dim);
          const std::size_t dim_index = (start / step) % dim_size;

          step_size_index[3 * dim] = step;
          step_size_index[3 * dim + 1] = dim_size;
          step_size_index[3 * dim + 2] = dim_index;

          start -= dim_index;
        }
      }

      bool operator==(array_iterator<T> &other) const {
        return (&parent == &other.parent) && (index == other.index);
      }

      array_iterator<T> &operator++() {
        std::size_t dim = ndim;
        for (; dim > 1; --dim) {
          const std::size_t dim_id = dim - 1;
          const std::size_t &dim_step = step_size_index[3 * dim_id];
          const std::size_t &dim_size = step_size_index[3 * dim_id + 1];
          std::size_t &dim_index = step_size_index[3 * dim_id + 2];

          index += dim_step;
          dim_index++;

          if (dim_index < dim_size) {
            return *this;
          }

          index -= dim_step * dim_index;
          dim_index = 0;
        }

        const std::size_t dim_id = 0;
        const std::size_t &dim_step = step_size_index[3 * dim_id];
        std::size_t &dim_index = step_size_index[3 * dim_id + 2];
        index += dim_step;
        dim_index++;

        return *this;
      }

      T &operator*() { return parent.mut_data()[index]; }
    };

    template <PrimativeTypes T> struct const_array_iterator {
    private:
      static constexpr std::size_t MAX_DIM = array<T>::MAX_DIM;
      const array<T> &parent;
      std::size_t index;
      const std::size_t ndim;
      std::array<std::size_t, 3 * MAX_DIM> step_size_index;

    public:
      const_array_iterator(const array<T> &arr, std::size_t start)
          : parent(arr), index(start), ndim(arr.ndim()) {
        step_size_index.fill(std::size_t());

        for (std::size_t dim = 0; dim < arr.ndim(); dim++) {
          const std::size_t step = arr.strides(dim) / sizeof(T);
          const std::size_t dim_size = arr.shape(dim);
          const std::size_t dim_index = (start / step) % dim_size;

          step_size_index[3 * dim] = step;
          step_size_index[3 * dim + 1] = dim_size;
          step_size_index[3 * dim + 2] = dim_index;

          start -= dim_index;
        }
      }

      bool operator==(const_array_iterator<T> &other) {
        return (&parent == &other.parent) && (index == other.index);
      }

      const_array_iterator<T> &operator++() {
        std::size_t dim = ndim;
        for (; dim > 1; --dim) {
          const std::size_t dim_id = dim - 1;
          const std::size_t &dim_step = step_size_index[3 * dim_id];
          const std::size_t &dim_size = step_size_index[3 * dim_id + 1];
          std::size_t &dim_index = step_size_index[3 * dim_id + 2];

          index += dim_step;
          dim_index++;

          if (dim_index < dim_size) {
            return *this;
          }

          index -= dim_step * dim_index;
          dim_index = 0;
        }

        const std::size_t dim_id = 0;
        const std::size_t &dim_step = step_size_index[3 * dim_id];
        std::size_t &dim_index = step_size_index[3 * dim_id + 2];
        index += dim_step;
        dim_index++;

        return *this;
      }

      const T &operator*() { return parent.data()[index]; }
    };

  }  // namespace details
}  // namespace quspin
