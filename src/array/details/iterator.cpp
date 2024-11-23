#include <quspin/array/details/iterator.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <vector>

namespace quspin {
  namespace details {

    template <PrimativeTypes T>
    array_iterator<T>::array_iterator(T *data,
                                      const std::vector<std::size_t> &shape,
                                      const std::vector<std::size_t> &strides,
                                      std::size_t start)
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

    template <PrimativeTypes T>
    bool array_iterator<T>::operator==(const array_iterator<T> &other) const {
      return (data_ == other.data_) && (index_ == other.index_);
    }

    template <PrimativeTypes T>
    array_iterator<T> &array_iterator<T>::operator++() {
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

    template <PrimativeTypes T>
    array_iterator<T> array_iterator<T>::operator++(int) {
      array_iterator<T> temp = *this;
      ++(*this);
      return temp;
    }

    template <PrimativeTypes T> T &array_iterator<T>::operator*() const {
      return const_cast<T &>(data_[index_]);
    }

    template struct array_iterator<int8_t>;
    template struct array_iterator<uint8_t>;
    template struct array_iterator<int16_t>;
    template struct array_iterator<uint16_t>;
    template struct array_iterator<int32_t>;
    template struct array_iterator<uint32_t>;
    template struct array_iterator<int64_t>;
    template struct array_iterator<uint64_t>;
    template struct array_iterator<float>;
    template struct array_iterator<double>;
    template struct array_iterator<cfloat>;
    template struct array_iterator<cdouble>;

  }  // namespace details
}  // namespace quspin
