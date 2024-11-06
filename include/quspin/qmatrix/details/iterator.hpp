#pragma once

#include <quspin/details/type_concepts.hpp>

namespace quspin {
  namespace details {
    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J> struct row_ref {
    private:
      T &value_;
      I &index_;
      J &cindex_;

    public:
      row_ref(T &value, I &index, J &cindex) : value_(value), index_(index), cindex_(cindex) {};
      bool operator==(const row_ref &other) const;
      bool operator!=(const row_ref &other) const;
      bool operator<(const row_ref &other) const;
      bool operator>(const row_ref &other) const;
      bool operator<=(const row_ref &other) const;
      bool operator>=(const row_ref &other) const;
    };

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J> struct iterator {
    private:
      T *data_ = nullptr;
      I *indices_ = nullptr;
      J *cindices_ = nullptr;

    public:
      iterator(T &data, I &indices, J &cindices)
          : data_(data), indices_(indices), cindices_(cindices) {}

      bool operator==(const iterator<T, I, J> &other) const;
      bool operator!=(const iterator<T, I, J> &other) const;
      bool operator<(const iterator<T, I, J> &other) const;
      bool operator>(const iterator<T, I, J> &other) const;
      bool operator<=(const iterator<T, I, J> &other) const;
      bool operator>=(const iterator<T, I, J> &other) const;

      iterator &operator++();
      iterator &operator--();
      iterator operator++(int);
      iterator operator--(int);

      iterator &operator+=(const std::size_t &n);
      iterator &operator-=(const std::size_t &n);
      iterator operator+(const std::size_t &n) const;
      iterator operator-(const std::size_t &n) const;

      row_ref<T, I, J> operator*();
      row_ref<const T, const I, const J> operator[](const std::size_t &n) const;
    };

  }  // namespace details
}  // namespace quspin
