#include <quspin/details/type_concepts.hpp>
#include <quspin/qmatrix/details/iterator.hpp>

namespace quspin {
  namespace details {

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    bool row_ref<T, I, J>::operator==(const row_ref<T, I, J> &other) const {
      return index_ == other.index_ && cindex_ == other.cindex_;
    }

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    bool row_ref<T, I, J>::operator!=(const row_ref<T, I, J> &other) const {
      return index_ != other.index_ || cindex_ != other.cindex_;
    }

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    bool row_ref<T, I, J>::operator<(const row_ref<T, I, J> &other) const {
      return index_ < other.index_ && cindex_ < other.cindex_;
    }

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    bool row_ref<T, I, J>::operator>(const row_ref<T, I, J> &other) const {
      return index_ > other.index_ && cindex_ > other.cindex_;
    }

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    bool row_ref<T, I, J>::operator<=(const row_ref<T, I, J> &other) const {
      return *this < other || *this == other;
    }

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    bool row_ref<T, I, J>::operator>=(const row_ref<T, I, J> &other) const {
      return *this > other || *this == other;
    }

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    bool iterator<T, I, J>::operator==(const iterator<T, I, J> &other) const {
      return data_ == other.data_ && indices_ == other.indices_ && cindices_ == other.cindices_;
    }

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    bool iterator<T, I, J>::operator!=(const iterator<T, I, J> &other) const {
      return data_ != other.data_ || indices_ != other.indices_ || cindices_ != other.cindices_;
    }

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    bool iterator<T, I, J>::operator<(const iterator<T, I, J> &other) const {
      return data_ < other.data_ && indices_ < other.indices_ && cindices_ < other.cindices_;
    }

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    bool iterator<T, I, J>::operator>(const iterator<T, I, J> &other) const {
      return data_ > other.data_ && indices_ > other.indices_ && cindices_ > other.cindices_;
    }

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    bool iterator<T, I, J>::operator<=(const iterator<T, I, J> &other) const {
      return *this < other || *this == other;
    }

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    bool iterator<T, I, J>::operator>=(const iterator<T, I, J> &other) const {
      return *this > other || *this == other;
    }

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    iterator<T, I, J> &iterator<T, I, J>::operator++() {
      data_++;
      indices_++;
      cindices_++;
      return *this;
    }

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    iterator<T, I, J> &iterator<T, I, J>::operator--() {
      data_--;
      indices_--;
      cindices_--;
      return *this;
    }

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    iterator<T, I, J> iterator<T, I, J>::operator++(int) {
      iterator temp = *this;
      ++*this;
      return temp;
    }

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    iterator<T, I, J> iterator<T, I, J>::operator--(int) {
      iterator temp = *this;
      --*this;
      return temp;
    }

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    iterator<T, I, J> &iterator<T, I, J>::operator+=(const std::size_t &n) {
      data_ += n;
      indices_ += n;
      cindices_ += n;
      return *this;
    }

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    iterator<T, I, J> &iterator<T, I, J>::operator-=(const std::size_t &n) {
      data_ -= n;
      indices_ -= n;
      cindices_ -= n;
      return *this;
    }

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    iterator<T, I, J> iterator<T, I, J>::operator+(const std::size_t &n) const {
      iterator temp = *this;
      temp += n;
      return temp;
    }

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    iterator<T, I, J> iterator<T, I, J>::operator-(const std::size_t &n) const {
      iterator temp = *this;
      temp -= n;
      return temp;
    }

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    row_ref<T, I, J> iterator<T, I, J>::operator*() {
      return row_ref(*data_, *indices_, *cindices_);
    }

    template <PrimativeTypes T, PrimativeTypes I, PrimativeTypes J>
    row_ref<const T, const I, const J> iterator<T, I, J>::operator[](const std::size_t &n) const {
      auto temp = *this + n;
      return iterator<const T, const I, const J>(*temp.data_, *temp.indices_, *temp.cindices_);
    }

  }  // namespace details
}  // namespace quspin
