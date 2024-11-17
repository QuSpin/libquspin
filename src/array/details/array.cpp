
#include <algorithm>
#include <cassert>
#include <numeric>
#include <quspin/array/details/array.hpp>
#include <quspin/details/type_concepts.hpp>
#include <vector>

namespace quspin {
  namespace details {

    template <PrimativeTypes T>
    std::size_t array<T>::get_ndim(const std::vector<std::size_t> &shape) {
      return shape.size();
    }

    template <PrimativeTypes T>
    std::size_t array<T>::get_size(const std::vector<std::size_t> &shape) {
      return (shape.size() > 0 ? std::reduce(shape.begin(), shape.end(), std::size_t(1),
                                             std::multiplies<std::size_t>())
                               : 0);
    }

    template <PrimativeTypes T>
    std::vector<std::size_t> array<T>::get_strides(const std::vector<std::size_t> &shape) {
      std::vector<std::size_t> &&strides = std::vector<std::size_t>(shape.size());
      if (shape.size() == 0) {
        return std::move(strides);
      }

      strides[shape.size() - 1] = sizeof(T);
      for (int i = static_cast<int>(shape.size()) - 2; i >= 0; i--) {
        strides[i] = shape[i + 1] * strides[i + 1];
      }
      return std::move(strides);
    }

    template <PrimativeTypes T>
    bool array<T>::check_contiguous(const std::vector<std::size_t> &stride,
                                    const std::vector<std::size_t> &shape) {
      if (stride.size() != shape.size()) {
        throw std::invalid_argument("Stride and shape must have the same size");
      }

      if (stride.size() == 0) {  // empty array
        return true;
      }

      const std::size_t ndim = stride.size();
      std::size_t last_stride = stride[ndim - 1];

      if (last_stride != sizeof(T)) {
        return false;
      }

      if (ndim == 1) {
        return true;
      }
      for (std::size_t i = ndim - 1; i > 0; i--) {
        if (stride[i - 1] != last_stride * shape[i]) {
          return false;
        }
        last_stride = stride[i];
      }
      return true;
    }

    template <PrimativeTypes T>
    std::size_t array<T>::get_offset(const std::vector<std::size_t> &input) const {
      if (input.size() != shape_.size()) {
        throw std::runtime_error("Invalid number of indices");
      }
      std::size_t offset = 0;
      for (std::size_t i = 0; i < ndim_; i++) {
        if (input[i] >= shape_[i]) {
          throw std::runtime_error("Index " + std::to_string(i) + " out of bounds: "
                                   + std::to_string(input[i]) + " >= " + std::to_string(shape_[i]));
        }
        offset += input[i] * stride_[i];
      }
      return offset / sizeof(T);
    }

    template <PrimativeTypes T> array<T>::array()
        : data_(reference_counted_ptr<T>()),
          stride_({}),
          shape_({}),
          size_(0),
          ndim_(0),
          is_contiguous_(true) {}

    template <PrimativeTypes T> array<T>::array(const std::vector<std::size_t> &shape)
        : data_(reference_counted_ptr<T>(get_size(shape))),
          stride_(get_strides(shape)),
          shape_(shape),
          size_(get_size(shape)),
          ndim_(get_ndim(shape)),
          is_contiguous_(true) {}

    template <PrimativeTypes T> array<T>::array(const std::vector<std::size_t> &shape,
                                                const std::vector<std::size_t> &stride, T *data)
        : data_(reference_counted_ptr<T>(data)),
          stride_(stride),
          shape_(shape),
          size_(get_size(shape)),
          ndim_(get_ndim(shape)),
          is_contiguous_(check_contiguous(stride, shape)) {}

    template <PrimativeTypes T> array<T>::array(std::initializer_list<T> values)
        : data_(reference_counted_ptr<T>(values.size())),
          stride_({sizeof(T)}),
          shape_({values.size()}),
          size_(values.size()),
          ndim_(1),
          is_contiguous_(true) {
      std::copy(values.begin(), values.end(), begin());
    }

    template <PrimativeTypes T> const T *array<T>::data() const { return data_.get(); }
    template <PrimativeTypes T> T *array<T>::mut_data() { return data_.get(); }

    template <PrimativeTypes T> array_iterator<T> array<T>::begin() const {
      array_iterator<T> iter(const_cast<T *>(data()), shape_, stride_, 0);
      return iter;
    }

    template <PrimativeTypes T> array_iterator<T> array<T>::end() const {
      array_iterator<T> iter(const_cast<T *>(data()), shape_, stride_, size());
      return iter;
    }

    template <PrimativeTypes T> array_iterator<T> array<T>::begin() {
      array_iterator<T> iter(mut_data(), shape_, stride_, 0);
      return iter;
    }

    template <PrimativeTypes T> array_iterator<T> array<T>::end() {
      array_iterator<T> iter(mut_data(), shape_, stride_, size());
      return iter;
    }

    template <PrimativeTypes T> bool array<T>::is_contiguous() const { return is_contiguous_; }

    template <PrimativeTypes T> std::size_t array<T>::strides(const std::size_t &i) const {
      return stride_.at(i);
    }
    template <PrimativeTypes T> std::size_t array<T>::shape(const std::size_t &i) const {
      return shape_.at(i);
    }
    template <PrimativeTypes T> std::vector<std::size_t> array<T>::shape() const { return shape_; }
    template <PrimativeTypes T> std::vector<std::size_t> array<T>::strides() const {
      return stride_;
    }

    template <PrimativeTypes T> std::size_t array<T>::ndim() const { return ndim_; }
    template <PrimativeTypes T> std::size_t array<T>::size() const { return size_; }
    template <PrimativeTypes T> std::size_t array<T>::itemsize() const { return sizeof(T); }
    template <PrimativeTypes T> std::size_t array<T>::use_count() const {
      return data_.use_count();
    }

    template <PrimativeTypes T> const T &array<T>::at(const std::vector<std::size_t> &input) const {
      return data()[get_offset(input)];
    }
    template <PrimativeTypes T> T &array<T>::at(const std::vector<std::size_t> &input) {
      return mut_data()[get_offset(input)];
    }

    template <PrimativeTypes T>
    const T &array<T>::at(std::initializer_list<std::size_t> &input) const {
      return at(std::vector<std::size_t>(input));
    }
    template <PrimativeTypes T> T &array<T>::at(std::initializer_list<std::size_t> &input) {
      return at(std::vector<std::size_t>(input));
    }

    template <PrimativeTypes T> array<T> array<T>::copy() const {
      array<T> out(shape_);
      std::copy(begin(), end(), out.begin());
      return out;
    }

    template struct array<int8_t>;
    template struct array<uint8_t>;
    template struct array<int16_t>;
    template struct array<uint16_t>;
    template struct array<int32_t>;
    template struct array<uint32_t>;
    template struct array<int64_t>;
    template struct array<uint64_t>;
    template struct array<float>;
    template struct array<double>;
    template struct array<cfloat>;
    template struct array<cdouble>;

  }  // namespace details
}  // namespace quspin
