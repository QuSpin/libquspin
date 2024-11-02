
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <numeric>
#include <quspin/array/details/array.hpp>
#include <quspin/array/details/iterator.hpp>
#include <quspin/array/details/pointer.hpp>
#include <quspin/details/operators.hpp>
#include <quspin/details/type_concepts.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <variant>
#include <vector>

namespace quspin {
  namespace details {

    template <PrimativeTypes T>
    std::size_t array<T>::get_offset(std::vector<std::size_t> &input) const {
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

    template <PrimativeTypes T>
    void array<T>::init_from_shape_(const std::vector<std::size_t> &shape) {
      shape_.resize(shape.size());
      std::copy(shape.begin(), shape.end(), shape_.begin());
      ndim_ = shape.size();
      size_ = (ndim_ > 0 ? std::reduce(shape.begin(), shape.end(), std::size_t(1),
                                       std::multiplies<std::size_t>())
                         : 0);

      stride_.resize(ndim_);
      if (ndim_ == 1) {
        stride_[0] = sizeof(T);
      } else if (ndim_ > 1) {
        stride_[ndim_ - 1] = sizeof(T);
        for (int i = static_cast<int>(ndim_) - 2; i >= 0; i--) {
          stride_[i] = shape[i + 1] * stride_[i + 1];
        }
      }
      data_ = reference_counted_ptr<T>(size_);
    }

    template <PrimativeTypes T>
    void array<T>::init_from_buffer_(const std::vector<std::size_t> &shape,
                                     const std::vector<std::size_t> &stride, T *data) {
      shape_.resize(shape.size());
      stride_.resize(stride.size());

      std::copy(shape.begin(), shape.end(), shape_.begin());
      std::copy(stride.begin(), stride.end(), stride_.begin());
      ndim_ = shape.size();
      size_ = (ndim_ > 0 ? std::reduce(shape.begin(), shape.end(), std::size_t(1),
                                       std::multiplies<std::size_t>())
                         : 0);

      if (ndim_ >= MAX_DIM) {
        std::stringstream err_stream;

        err_stream << "NDim must be smaller than ";
        err_stream << MAX_DIM;
        err_stream << " got a value of ";
        err_stream << ndim_;

        throw std::invalid_argument(err_stream.str());
      }

      // does not own the memory, do not delete
      data_ = reference_counted_ptr<T>(data, size_);
    }

    template <PrimativeTypes T> array<T>::array()
        : data_(reference_counted_ptr<T>()), stride_({}), shape_({}), size_(0), ndim_(0){};
    template <PrimativeTypes T> array<T>::array(const std::vector<std::size_t> &shape) {
      init_from_shape_(shape);
    }
    template <PrimativeTypes T> array<T>::array(const std::vector<std::size_t> &shape,
                                                const std::vector<std::size_t> &stride, T *data) {
      init_from_buffer_(shape, stride, data);
    }
    template <PrimativeTypes T> array<T>::array(std::initializer_list<T> values) {
      data_ = reference_counted_ptr<T>(values.size());
      stride_ = {sizeof(T)};
      shape_ = {values.size()};
      ndim_ = 1;
      size_ = values.size();

      std::copy(values.begin(), values.end(), this->begin());
    }

    template <PrimativeTypes T> const T *array<T>::data() const { return data_.get(); }
    template <PrimativeTypes T> T *array<T>::mut_data() { return data_.get(); }

    template <PrimativeTypes T> const_array_iterator<T> array<T>::begin() const {
      const_array_iterator<T> iter(*this, 0);
      return iter;
    }

    template <PrimativeTypes T> const_array_iterator<T> array<T>::end() const {
      const_array_iterator<T> iter(*this, size());
      return iter;
    }

    template <PrimativeTypes T> array_iterator<T> array<T>::begin() {
      array_iterator<T> iter(*this, 0);
      return iter;
    }

    template <PrimativeTypes T> array_iterator<T> array<T>::end() {
      array_iterator<T> iter(*this, size());
      return iter;
    }

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

    template <PrimativeTypes T> const T &array<T>::at(std::vector<std::size_t> &input) const {
      return data()[get_offset(input)];
    }
    template <PrimativeTypes T> T &array<T>::at(std::vector<std::size_t> &input) {
      return mut_data()[get_offset(input)];
    }

    template <PrimativeTypes T>
    const T &array<T>::at(std::initializer_list<std::size_t> &input) const {
      return this->at(std::vector<std::size_t>(input));
    }
    template <PrimativeTypes T> T &array<T>::at(std::initializer_list<std::size_t> &input) {
      return this->at(std::vector<std::size_t>(input));
    }
    template <PrimativeTypes T> array<T> array<T>::copy() const {
      array<T> out(shape_);
      std::copy(this->begin(), this->end(), out.begin());
      return out;
    }

    template struct array<int8_t>;
    template struct array<uint8_t>;
    template struct array<int16_t>;
    template struct array<uint16_t>;
    template struct array<uint32_t>;
    template struct array<int32_t>;
    template struct array<uint64_t>;
    template struct array<int64_t>;
    template struct array<float>;
    template struct array<double>;
    template struct array<cfloat>;
    template struct array<cdouble>;

  }  // namespace details
}  // namespace quspin
