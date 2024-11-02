#pragma once

#include <quspin/details/type_concepts.hpp>
#include <quspin/dtype/details/dtype.hpp>

namespace quspin {
  namespace details {

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
  }  // namespace details
}  // namespace quspin
