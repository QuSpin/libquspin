#pragma once

#include <array>
#include <quspin/details/type_concepts.hpp>
#include <vector>

namespace quspin {
  namespace details {

    template <PrimativeTypes T> struct array_iterator {
    private:
      static constexpr std::size_t MAX_DIM = 64;
      T *data_;
      std::size_t index_;
      std::size_t ndim_;
      std::array<std::size_t, 3 * MAX_DIM> step_size_index_;

    public:
      using difference_type = std::ptrdiff_t;
      using value_type = T;
      using reference_type = T &;
      using iterator_category = std::forward_iterator_tag;

      array_iterator() = default;
      array_iterator(const array_iterator<T> &other) = default;
      array_iterator(array_iterator<T> &&other) = default;
      array_iterator(T *data, const std::vector<std::size_t> &shape,
                     const std::vector<std::size_t> &strides, std::size_t start);

      array_iterator &operator=(const array_iterator<T> &other) = default;
      array_iterator &operator=(array_iterator<T> &&other) = default;

      bool operator==(const array_iterator<T> &other) const;
      array_iterator<T> &operator++();
      array_iterator<T> operator++(int);
      reference_type operator*();
      reference_type operator*() const;
    };

    static_assert(std::forward_iterator<array_iterator<int8_t>>,
                  "array_iterator is not a forward iterator");

  }  // namespace details
}  // namespace quspin
