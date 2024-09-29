#pragma once

#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <numeric>
#include <quspin/details/complex.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <stdexcept>
#include <type_traits>
#include <variant>
#include <vector>

namespace quspin {
  namespace details {

    template <typename T> struct reference_counted_ptr : public typed_object<T> {
      T *ptr;
      ssize_t *ref_count_;
      bool owns_pointer;  // if true, the pointer can be deleted when ref_count_ == 0
      ssize_t size;

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
            ref_count_(new ssize_t(1)),
            owns_pointer(false),  // cannot delete nullptr
            size(0) {}

      reference_counted_ptr(T *ptr, ssize_t size)
          : ptr(ptr),
            ref_count_(new ssize_t(1)),
            owns_pointer(false),  // does not own the memory, do not delete
            size(size) {}

      reference_counted_ptr(ssize_t size)
          : ptr(new T[size]),
            ref_count_(new ssize_t(1)),
            owns_pointer(true),  // objects all own the memory, can delete
            size(size) {}

      reference_counted_ptr(const reference_counted_ptr &other)
          : ptr(other.ptr),
            ref_count_(other.ref_count_),
            owns_pointer(other.owns_pointer),  // inherit ownership of memory
            size(other.size) {
        inc();
      }

      ~reference_counted_ptr() { dec(); }

      reference_counted_ptr<T> &operator=(const reference_counted_ptr<T> &other) {
        if (this != &other) {
          dec();
          ptr = other.ptr;
          ref_count_ = other.ref_count_;
          owns_pointer = other.owns_pointer;
          size = other.size;
          (*ref_count_)++;
        }
        return *this;
      }

      ssize_t use_count() const { return *ref_count_; }

      T *get() { return ptr; }

      const T *get() const { return static_cast<const T *>(ptr); }

      void *data() const { return reinterpret_cast<void *>(ptr); }
    };

    template <typename T> struct array : public typed_object<T> {
    private:
      reference_counted_ptr<T> data_;
      std::vector<ssize_t> stride_;
      std::vector<ssize_t> shape_;
      ssize_t size_;
      ssize_t ndim_;

      ssize_t get_offset(std::vector<ssize_t> &input) const {
        if (input.size() != shape_.size()) {
          throw std::runtime_error("Invalid number of indices");
        }
        ssize_t offset = 0;
        for (int i = 0; i < ndim_; i++) {
          if (input[i] < 0 || input[i] >= shape_[i]) {
            throw std::runtime_error("Index " + std::to_string(i) + " out of bounds");
          }
          offset += input[i] * stride_[i];
        }
        return offset / sizeof(T);
      }

      void init_from_stl_(const std::vector<ssize_t> &shape, T *data) {
        shape_ = shape;
        ndim_ = shape.size();
        size_ = (ndim_ > 0 ? std::reduce(shape.begin(), shape.end(), 1, std::multiplies<ssize_t>())
                           : 0);

        stride_.resize(ndim_);
        stride_[ndim_ - 1] = sizeof(T);
        for (int i = ndim_ - 2; i >= 0; i--) {
          stride_[i] = stride_[i + 1] * shape[i + 1] * sizeof(T);
        }

        if (data == nullptr) {
          data_ = reference_counted_ptr<T>(size_);
        } else {
          // does not own the memory, do not delete
          data_ = reference_counted_ptr<T>(data, size_);
          ;
        }
      }

    public:
      array() : data_(reference_counted_ptr<T>()), stride_({}), shape_({}), size_(0), ndim_(0) {};
      array(std::initializer_list<ssize_t> shape, T *data = nullptr) {
        init_from_stl_(std::vector<ssize_t>(shape), data);
      }
      array(const std::vector<ssize_t> &shape, T *data = nullptr) { init_from_stl_(shape, data); }

      const T *data() const { return data_.get(); }
      T *mut_data() { return data_.get(); }

      const T *cbegin() const { return data(); }
      const T *cend() const { return data() + size(); }

      T *begin() { return mut_data(); }
      T *end() { return mut_data() + size(); }

      ssize_t stride(int i) const { return stride_.at(i); }
      std::vector<ssize_t> shape() const { return shape_; }

      ssize_t shape(int i) const { return shape_.at(i); }
      std::vector<ssize_t> stride() const { return stride_; }

      ssize_t ndim() const { return ndim_; }
      ssize_t size() const { return size_; }
      ssize_t itemsize() const { return sizeof(T); }
      ssize_t use_count() const { return data_.use_count(); }

      const T &at(std::vector<ssize_t> &input) const { return data()[get_offset(input)]; }
      const T &at(std::initializer_list<ssize_t> &input) const {
        return at(std::vector<ssize_t>(input));
      }
      T &at(std::vector<ssize_t> &input) { return mut_data()[get_offset(input)]; }
      T &at(std::initializer_list<ssize_t> &input) { return at(std::vector<ssize_t>(input)); }
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