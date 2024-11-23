// Copyright 2024 Phillip Weinberg
#pragma once

#include <quspin/details/type_concepts.hpp>
#include <quspin/dtype/details/dtype.hpp>

namespace quspin {
namespace details {

template <PrimativeTypes T>
struct reference_counted_ptr : public typed_object<T> {
  T *ptr;
  std::size_t *ref_count_;
  bool owns_pointer;  // if true, the pointer can be deleted when ref_count_
                      // == 1

  void inc();
  void dec();

 public:
  reference_counted_ptr();
  reference_counted_ptr(T *ptr);
  reference_counted_ptr(std::size_t size);
  reference_counted_ptr(const reference_counted_ptr &other);
  ~reference_counted_ptr();

  reference_counted_ptr<T> &operator=(const reference_counted_ptr<T> &);
  std::size_t use_count() const;

  T *get();
  const T *get() const;
  void *data() const;
};
}  // namespace details
}  // namespace quspin
