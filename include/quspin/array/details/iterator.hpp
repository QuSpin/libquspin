#pragma once

#include <quspin/details/type_concepts.hpp>
#include <vector>

namespace quspin {
  namespace details {

    template <PrimativeTypes T> struct array;

    template <PrimativeTypes T> struct array_iterator {
    private:
      static constexpr std::size_t MAX_DIM = 64;
      T *data_;
      std::size_t index_;
      const std::size_t ndim_;

      std::array<std::size_t, 3 * MAX_DIM> step_size_index_;

    public:
      array_iterator(T *data, const std::vector<std::size_t> &shape,
                     const std::vector<std::size_t> &strides, std::size_t start)
          : data_(data), index_(start), ndim_(shape.size()) {
        step_size_index_.fill(std::size_t());

        for (std::size_t dim = 0; dim < ndim_; dim++) {
          const std::size_t step = strides.at(dim) / sizeof(T);
          const std::size_t dim_size = shape.at(dim);
          const std::size_t dim_index = (start / step) % dim_size;

          step_size_index_[3 * dim] = step;
          step_size_index_[3 * dim + 1] = dim_size;
          step_size_index_[3 * dim + 2] = dim_index;

          start -= dim_index;
        }
      }

      bool operator==(array_iterator<T> &other) const {
        return (data_ == other.data_) && (index_ == other.index_);
      }

      array_iterator<T> &operator++() {
        std::size_t dim = ndim_;
        for (; dim > 1; --dim) {
          const std::size_t dim_id = dim - 1;
          const std::size_t &dim_step = step_size_index_[3 * dim_id];
          const std::size_t &dim_size = step_size_index_[3 * dim_id + 1];
          std::size_t &dim_index = step_size_index_[3 * dim_id + 2];

          index_ += dim_step;
          dim_index++;

          if (dim_index < dim_size) {
            return *this;
          }

          index_ -= dim_step * dim_index;
          dim_index = 0;
        }

        const std::size_t dim_id = 0;
        const std::size_t &dim_step = step_size_index_[3 * dim_id];
        std::size_t &dim_index = step_size_index_[3 * dim_id + 2];
        index_ += dim_step;
        dim_index++;

        return *this;
      }

      T &operator*() { return data_[index_]; }
    };

    template <PrimativeTypes T> struct const_array_iterator {
    private:
      static constexpr std::size_t MAX_DIM = 64;
      const T *data_;
      std::size_t index_;
      const std::size_t ndim_;
      std::array<std::size_t, 3 * MAX_DIM> step_size_index_;

    public:
      const_array_iterator(const T *data, const std::vector<std::size_t> &shape,
                           const std::vector<std::size_t> &strides, std::size_t start)
          : data_(data), index_(start), ndim_(shape.size()) {
        step_size_index_.fill(std::size_t());

        for (std::size_t dim = 0; dim < ndim_; dim++) {
          const std::size_t step = strides.at(dim) / sizeof(T);
          const std::size_t dim_size = shape.at(dim);
          const std::size_t dim_index = (start / step) % dim_size;

          step_size_index_[3 * dim] = step;
          step_size_index_[3 * dim + 1] = dim_size;
          step_size_index_[3 * dim + 2] = dim_index;

          start -= dim_index;
        }
      }

      bool operator==(const_array_iterator<T> &other) {
        return (data_ == other.data_) && (index_ == other.index_);
      }

      const_array_iterator<T> &operator++() {
        std::size_t dim = ndim_;
        for (; dim > 1; --dim) {
          const std::size_t dim_id = dim - 1;
          const std::size_t &dim_step = step_size_index_[3 * dim_id];
          const std::size_t &dim_size = step_size_index_[3 * dim_id + 1];
          std::size_t &dim_index = step_size_index_[3 * dim_id + 2];

          index_ += dim_step;
          dim_index++;

          if (dim_index < dim_size) {
            return *this;
          }

          index_ -= dim_step * dim_index;
          dim_index = 0;
        }

        const std::size_t dim_id = 0;
        const std::size_t &dim_step = step_size_index_[3 * dim_id];
        std::size_t &dim_index = step_size_index_[3 * dim_id + 2];
        index_ += dim_step;
        dim_index++;

        return *this;
      }

      const T &operator*() { return data_[index_]; }
    };

  }  // namespace details
}  // namespace quspin
