
#include <quspin/array/details/pointer.hpp>
#include <quspin/details/type_concepts.hpp>

namespace quspin {
  namespace details {

    template <PrimativeTypes T> void reference_counted_ptr<T>::inc() { (*ref_count_)++; }

    template <PrimativeTypes T> void reference_counted_ptr<T>::dec() {
      if (*ref_count_ == 1) {
        delete ref_count_;
        if (owns_pointer) {
          delete[] ptr;
        }
      } else {
        (*ref_count_)--;
      }
    }

    template <PrimativeTypes T> reference_counted_ptr<T>::reference_counted_ptr()
        : ptr(nullptr),
          ref_count_(new std::size_t(1)),
          owns_pointer(false),  // cannot delete nullptr
          size(0) {}

    template <PrimativeTypes T>
    reference_counted_ptr<T>::reference_counted_ptr(T *ptr, std::size_t size)
        : ptr(ptr),
          ref_count_(new std::size_t(1)),
          owns_pointer(false),  // does not own the memory, do not delete
          size(size) {}

    template <PrimativeTypes T> reference_counted_ptr<T>::reference_counted_ptr(std::size_t size)
        : ptr(new T[size]),
          ref_count_(new std::size_t(1)),
          owns_pointer(true),  // objects all own the memory, can delete
          size(size) {
      std::fill(ptr, ptr + size, T());
    }

    template <PrimativeTypes T>
    reference_counted_ptr<T>::reference_counted_ptr(const reference_counted_ptr &other)
        : ptr(other.ptr),
          ref_count_(other.ref_count_),
          owns_pointer(other.owns_pointer),  // inherit ownership of memory
          size(other.size) {
      inc();
    }

    template <PrimativeTypes T> reference_counted_ptr<T>::~reference_counted_ptr() { dec(); }

    template <PrimativeTypes T> reference_counted_ptr<T> &reference_counted_ptr<T>::operator=(
        const reference_counted_ptr<T> &other) {
      dec();
      ptr = other.ptr;
      ref_count_ = other.ref_count_;
      owns_pointer = other.owns_pointer;
      size = other.size;
      inc();

      return *this;
    }

    template <PrimativeTypes T> std::size_t reference_counted_ptr<T>::use_count() const {
      return *ref_count_;
    }

    template <PrimativeTypes T> T *reference_counted_ptr<T>::get() { return ptr; }

    template <PrimativeTypes T> const T *reference_counted_ptr<T>::get() const {
      return static_cast<const T *>(ptr);
    }

    template <PrimativeTypes T> void *reference_counted_ptr<T>::data() const {
      return reinterpret_cast<void *>(ptr);
    }

    template struct reference_counted_ptr<int8_t>;
    template struct reference_counted_ptr<uint8_t>;
    template struct reference_counted_ptr<int16_t>;
    template struct reference_counted_ptr<uint16_t>;
    template struct reference_counted_ptr<uint32_t>;
    template struct reference_counted_ptr<int32_t>;
    template struct reference_counted_ptr<uint64_t>;
    template struct reference_counted_ptr<int64_t>;
    template struct reference_counted_ptr<float>;
    template struct reference_counted_ptr<double>;
    template struct reference_counted_ptr<cfloat>;
    template struct reference_counted_ptr<cdouble>;

  }  // namespace details
}  // namespace quspin
