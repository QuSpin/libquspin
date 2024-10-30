#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <numeric>
#include <quspin/details/operators.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <variant>
#include <vector>

namespace quspin {
  namespace details {

    template <PrimativeTypes T> struct array_iterator;
    template <PrimativeTypes T> struct const_array_iterator;

    template <PrimativeTypes T> struct reference_counted_ptr : public typed_object<T> {
      T *ptr;
      std::size_t *ref_count_;
      bool owns_pointer;  // if true, the pointer can be deleted when ref_count_ == 1
      std::size_t size;

      void inc() { (*ref_count_)++; }

      void dec() {
        if (*ref_count_ == 1) {
          delete ref_count_;
          if (owns_pointer) {
            delete[] ptr;
          }
        } else {
          (*ref_count_)--;
        }
      }

    public:
      reference_counted_ptr()
          : ptr(nullptr),
            ref_count_(new std::size_t(1)),
            owns_pointer(false),  // cannot delete nullptr
            size(0) {}

      reference_counted_ptr(T *ptr, std::size_t size)
          : ptr(ptr),
            ref_count_(new std::size_t(1)),
            owns_pointer(false),  // does not own the memory, do not delete
            size(size) {}

      reference_counted_ptr(std::size_t size)
          : ptr(new T[size]),
            ref_count_(new std::size_t(1)),
            owns_pointer(true),  // objects all own the memory, can delete
            size(size) {
        std::fill(ptr, ptr + size, T());
      }

      reference_counted_ptr(const reference_counted_ptr &other)
          : ptr(other.ptr),
            ref_count_(other.ref_count_),
            owns_pointer(other.owns_pointer),  // inherit ownership of memory
            size(other.size) {
        inc();
      }

      ~reference_counted_ptr() { dec(); }

      reference_counted_ptr<T> &operator=(const reference_counted_ptr<T> &other) {
        dec();
        ptr = other.ptr;
        ref_count_ = other.ref_count_;
        owns_pointer = other.owns_pointer;
        size = other.size;
        inc();

        return *this;
      }

      std::size_t use_count() const { return *ref_count_; }

      T *get() { return ptr; }

      const T *get() const { return static_cast<const T *>(ptr); }

      void *data() const { return reinterpret_cast<void *>(ptr); }
    };

    template <PrimativeTypes T> struct array : public typed_object<T> {
    private:
      reference_counted_ptr<T> data_;
      std::vector<std::size_t> stride_;
      std::vector<std::size_t> shape_;
      std::size_t size_;
      std::size_t ndim_;

      std::size_t get_offset(std::vector<std::size_t> &input) const {
        if (input.size() != shape_.size()) {
          throw std::runtime_error("Invalid number of indices");
        }
        std::size_t offset = 0;
        for (std::size_t i = 0; i < ndim_; i++) {
          if (input[i] >= shape_[i]) {
            throw std::runtime_error("Index " + std::to_string(i)
                                     + " out of bounds: " + std::to_string(input[i])
                                     + " >= " + std::to_string(shape_[i]));
          }
          offset += input[i] * stride_[i];
        }
        return offset / sizeof(T);
      }

      void init_from_shape_(const std::vector<std::size_t> &shape) {
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
            stride_[i] = shape[i + 1] * sizeof(T);
          }
        }
        data_ = reference_counted_ptr<T>(size_);
      }

      void init_from_buffer_(const std::vector<std::size_t> &shape,
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

    public:
      static constexpr std::size_t MAX_DIM = 64;

      array() : data_(reference_counted_ptr<T>()), stride_({}), shape_({}), size_(0), ndim_(0) {};
      array(const std::vector<std::size_t> &shape) { init_from_shape_(shape); }
      array(const std::vector<std::size_t> &shape, const std::vector<std::size_t> &stride,
            T *data) {
        init_from_buffer_(shape, stride, data);
      }
      array(std::initializer_list<T> values) {
        data_ = reference_counted_ptr<T>(values.size());
        stride_ = {sizeof(T)};
        shape_ = {values.size()};
        ndim_ = 1;
        size_ = values.size();

        std::copy(values.begin(), values.end(), begin());
      }

      const T *data() const { return data_.get(); }
      T *mut_data() { return data_.get(); }

      const_array_iterator<T> begin() const;
      const_array_iterator<T> end() const;
      array_iterator<T> begin();
      array_iterator<T> end();

      std::size_t strides(const std::size_t &i) const { return stride_.at(i); }
      std::size_t shape(const std::size_t &i) const { return shape_.at(i); }
      std::vector<std::size_t> shape() const { return shape_; }
      std::vector<std::size_t> strides() const { return stride_; }

      std::size_t ndim() const { return ndim_; }
      std::size_t size() const { return size_; }
      std::size_t itemsize() const { return sizeof(T); }
      std::size_t use_count() const { return data_.use_count(); }

      const T &at(std::vector<std::size_t> &input) const { return data()[get_offset(input)]; }
      const T &at(std::initializer_list<std::size_t> &input) const {
        return at(std::vector<std::size_t>(input));
      }
      T &at(std::vector<std::size_t> &input) { return mut_data()[get_offset(input)]; }
      T &at(std::initializer_list<std::size_t> &input) {
        return at(std::vector<std::size_t>(input));
      }
      array<T> copy() const {
        array<T> out(shape_);
        std::copy(begin(), end(), out.begin());
        return out;
      }
    };

    template <PrimativeTypes T> struct array_iterator {
    private:
      static constexpr std::size_t MAX_DIM = array<T>::MAX_DIM;
      array<T> &parent;
      std::size_t index;
      std::size_t dim;

      std::array<std::size_t, MAX_DIM> dim_indices;
      std::array<std::size_t, MAX_DIM> steps;

    public:
      array_iterator(array<T> &arr, std::size_t start) : parent(arr), index(start) {
        std::fill(steps.begin(), steps.end(), std::size_t());
        std::fill(dim_indices.begin(), dim_indices.end(), std::size_t());

        for (std::size_t dim = 0; dim < arr.ndim(); dim++) {
          const std::size_t step = arr.strides(dim) / sizeof(T);

          std::size_t dim_index = (start / step) % arr.shape(dim);

          steps[dim] = step;
          dim_indices[dim] = dim_index;

          start -= dim_index;
        }
      }

      bool operator==(array_iterator<T> &other) const {
        return (&parent == &other.parent) && (index == other.index);
      }

      array_iterator<T> &operator++() {
        for (std::size_t dim = parent.ndim(); dim > 0; --dim) {
          const std::size_t dim_id = dim - 1;
          const std::size_t dim_size = parent.shape(dim_id);

          if (dim_indices[dim_id] < (dim_size - 1)) {
            dim_indices[dim_id]++;
            index += steps[dim_id];
            return *this;
          } else if (dim_id > 1) {
            index -= steps[dim_id] * (dim_size - 1);
            dim_indices[dim_id] = 0;
          }
        }
        index++;
        return *this;
      }

      std::size_t index_() const { return index; }

      T &operator*() { return parent.mut_data()[index]; }
    };

    template <PrimativeTypes T> struct const_array_iterator {
    private:
      static constexpr std::size_t MAX_DIM = array<T>::MAX_DIM;
      const array<T> &parent;
      std::size_t index;
      std::array<std::size_t, MAX_DIM> dim_indices;
      std::array<std::size_t, MAX_DIM> steps;

    public:
      const_array_iterator(const array<T> &arr, std::size_t start) : parent(arr), index(start) {
        std::fill(steps.begin(), steps.end(), std::size_t());
        std::fill(dim_indices.begin(), dim_indices.end(), std::size_t());

        assert(arr.ndim() == arr.shape().size());
        assert(arr.ndim() == arr.strides().size());

        for (std::size_t dim = 0; dim < arr.ndim(); dim++) {
          const std::size_t step = arr.strides(dim) / sizeof(T);

          std::size_t dim_index = (start / step) % arr.shape(dim);

          assert(dim_index >= 0 && dim_index < arr.shape(dim));
          steps[dim] = step;
          dim_indices[dim] = dim_index;

          start -= dim_index;
        }
      }

      bool operator!=(const_array_iterator<T> &other) {
        return (&parent != &other.parent) || (index != other.index);
      }

      const_array_iterator<T> &operator++() {
        for (std::size_t dim = parent.ndim(); dim > 0; --dim) {
          const std::size_t dim_id = dim - 1;
          const std::size_t dim_size = parent.shape(dim_id);

          if (dim_indices[dim_id] < (dim_size - 1)) {
            dim_indices[dim_id]++;
            index += steps[dim_id];
            return *this;
          }

          if (dim_id > 1) {
            index -= steps[dim_id] * (dim_size - 1);
            dim_indices[dim_id] = 0;
          }
        }
        index++;
        return *this;
      }

      const T &operator*() { return parent.data()[index]; }
    };

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

    template <typename T> struct value_type<array<T>> {
      using type = T;
    };

    template <typename T> constexpr bool is_integer_array_v
        = std::is_integral_v<typename array<T>::value_type>;

    template <typename T> constexpr bool is_floating_array_v
        = std::is_floating_point_v<typename array<T>::value_type>;

    template <typename T> constexpr bool is_complex_array_v
        = std::conjunction_v<std::is_same<typename array<T>::value_type, cfloat>,
                             std::is_same<typename array<T>::value_type, cdouble>,
                             std::is_same<typename array<T>::value_type, cldouble>>;

    template <typename U, typename T> constexpr bool is_array_type_v
        = std::is_same_v<U, typename array<T>::value_type>;

    using arrays = std::variant<array<int8_t>, array<uint8_t>, array<int16_t>, array<uint16_t>,
                                array<uint32_t>, array<int32_t>, array<uint64_t>, array<int64_t>,
                                array<float>, array<double>, array<cfloat>, array<cdouble>>;

    using index_arrays = std::variant<array<int32_t>, array<int64_t>>;
    using matrix_arrays
        = std::variant<array<int8_t>, array<float>, array<double>, array<cfloat>, array<cdouble>>;
    using state_arrays = std::variant<array<double>, array<cdouble>>;

    template <typename T> constexpr bool is_index_type_v
        = std::conjunction_v<std::is_same<T, int32_t>, std::is_same<T, int64_t>>;

    template <typename T> constexpr bool is_matrix_type_v
        = std::conjunction_v<std::is_same<T, int8_t>, std::is_same<T, float>,
                             std::is_same<T, double>, std::is_same<T, cfloat>,
                             std::is_same<T, cdouble>>;

    template <typename T> constexpr bool is_state_type_v
        = std::conjunction_v<std::is_same<T, double>, std::is_same<T, cdouble>>;

  }  // namespace details
}  // namespace quspin
