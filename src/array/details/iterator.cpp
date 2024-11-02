
#include <quspin/array/details/iterator.hpp>
#include <quspin/details/type_concepts.hpp>
#include <quspin/dtype/details/dtype.hpp>

namespace quspin {
  namespace details {
    template <PrimativeTypes T> struct array;

    template <PrimativeTypes T> array_iterator<T>::array_iterator(array<T> &arr, std::size_t start)
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

    template <PrimativeTypes T> bool array_iterator<T>::operator==(array_iterator<T> &other) const {
      return (&parent == &other.parent) && (index == other.index);
    }

    template <PrimativeTypes T> array_iterator<T> &array_iterator<T>::operator++() {
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

    template <PrimativeTypes T> T &array_iterator<T>::operator*() {
      return parent.mut_data()[index];
    }

    template <PrimativeTypes T>
    const_array_iterator<T>::const_array_iterator(const array<T> &arr, std::size_t start)
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

    template <PrimativeTypes T>
    bool const_array_iterator<T>::operator==(const_array_iterator<T> &other) const {
      return (&parent == &other.parent) && (index == other.index);
    }

    template <PrimativeTypes T> const_array_iterator<T> &const_array_iterator<T>::operator++() {
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

    template <PrimativeTypes T> const T &const_array_iterator<T>::operator*() {
      return parent.data()[index];
    }

    template struct array_iterator<int8_t>;
    template struct array_iterator<uint8_t>;
    template struct array_iterator<int16_t>;
    template struct array_iterator<uint16_t>;
    template struct array_iterator<uint32_t>;
    template struct array_iterator<int32_t>;
    template struct array_iterator<uint64_t>;
    template struct array_iterator<int64_t>;
    template struct array_iterator<float>;
    template struct array_iterator<double>;
    template struct array_iterator<cfloat>;
    template struct array_iterator<cdouble>;
    template struct const_array_iterator<int8_t>;
    template struct const_array_iterator<uint8_t>;
    template struct const_array_iterator<int16_t>;
    template struct const_array_iterator<uint16_t>;
    template struct const_array_iterator<uint32_t>;
    template struct const_array_iterator<int32_t>;
    template struct const_array_iterator<uint64_t>;
    template struct const_array_iterator<int64_t>;
    template struct const_array_iterator<float>;
    template struct const_array_iterator<double>;
    template struct const_array_iterator<cfloat>;
    template struct const_array_iterator<cdouble>;

  }  // namespace details
}  // namespace quspin
