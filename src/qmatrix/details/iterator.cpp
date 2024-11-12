#include <quspin/details/type_concepts.hpp>
#include <quspin/dtype/details/dtype.hpp>
#include <quspin/qmatrix/details/iterator.hpp>

namespace quspin {
  namespace details {

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    row_info<T, I, J> &row_info<T, I, J>::operator=(row_info<T, I, J> &other) {
      value = other.value;
      index = other.index;
      cindex = other.cindex;
      return *this;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    row_info<T, I, J> &row_info<T, I, J>::operator=(row_info<T, I, J> &&other) {
      value = other.value;
      index = other.index;
      cindex = other.cindex;
      return *this;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    bool row_info<T, I, J>::operator==(const row_info<T, I, J> &other) const {
      return index == other.index && cindex == other.cindex;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    bool row_info<T, I, J>::operator!=(const row_info<T, I, J> &other) const {
      return index != other.index || cindex != other.cindex;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    bool row_info<T, I, J>::operator<(const row_info<T, I, J> &other) const {
      return index < other.index && cindex < other.cindex;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    bool row_info<T, I, J>::operator>(const row_info<T, I, J> &other) const {
      return index > other.index && cindex > other.cindex;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    bool row_info<T, I, J>::operator<=(const row_info<T, I, J> &other) const {
      return *this < other || *this == other;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    bool row_info<T, I, J>::operator>=(const row_info<T, I, J> &other) const {
      return *this > other || *this == other;
    }

    // iterator implementation

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    T iterator<T, I, J>::value() const {
      return *data_;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    I iterator<T, I, J>::index() const {
      return *indices_;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    J iterator<T, I, J>::cindex() const {
      return *cindices_;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    bool iterator<T, I, J>::operator==(const iterator<T, I, J> &other) const {
      return data_ == other.data_ && indices_ == other.indices_ && cindices_ == other.cindices_;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    bool iterator<T, I, J>::operator!=(const iterator<T, I, J> &other) const {
      return data_ != other.data_ || indices_ != other.indices_ || cindices_ != other.cindices_;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    bool iterator<T, I, J>::operator<(const iterator<T, I, J> &other) const {
      return data_ < other.data_ && indices_ < other.indices_ && cindices_ < other.cindices_;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    bool iterator<T, I, J>::operator>(const iterator<T, I, J> &other) const {
      return data_ > other.data_ && indices_ > other.indices_ && cindices_ > other.cindices_;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    bool iterator<T, I, J>::operator<=(const iterator<T, I, J> &other) const {
      return *this < other || *this == other;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    bool iterator<T, I, J>::operator>=(const iterator<T, I, J> &other) const {
      return *this > other || *this == other;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    iterator<T, I, J> &iterator<T, I, J>::operator++() {
      data_++;
      indices_++;
      cindices_++;
      return *this;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    iterator<T, I, J> &iterator<T, I, J>::operator--() {
      data_--;
      indices_--;
      cindices_--;
      return *this;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    iterator<T, I, J> iterator<T, I, J>::operator++(int) {
      iterator temp = *this;
      ++*this;
      return temp;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    iterator<T, I, J> iterator<T, I, J>::operator--(int) {
      iterator temp = *this;
      --*this;
      return temp;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    iterator<T, I, J> &iterator<T, I, J>::operator+=(const iterator<T, I, J>::difference_type &n) {
      data_ += n;
      indices_ += n;
      cindices_ += n;
      return *this;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    iterator<T, I, J> &iterator<T, I, J>::operator-=(const iterator<T, I, J>::difference_type &n) {
      data_ -= n;
      indices_ -= n;
      cindices_ -= n;
      return *this;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    iterator<T, I, J> iterator<T, I, J>::operator+(const iterator<T, I, J>::difference_type &n) {
      iterator temp(this->data_ + n, this->indices_ + n, this->cindices_ + n);
      return temp;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    iterator<T, I, J> iterator<T, I, J>::operator-(const iterator<T, I, J>::difference_type &n) {
      iterator temp(this->data_ - n, this->indices_ - n, this->cindices_ - n);
      return temp;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    iterator<T, I, J> iterator<T, I, J>::operator+(
        const iterator<T, I, J>::difference_type &n) const {
      iterator temp(this->data_ + n, this->indices_ + n, this->cindices_ + n);
      return temp;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    iterator<T, I, J> iterator<T, I, J>::operator-(
        const iterator<T, I, J>::difference_type &n) const {
      iterator temp(this->data_ - n, this->indices_ - n, this->cindices_ - n);
      return temp;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    iterator<T, I, J>::difference_type iterator<T, I, J>::operator-(
        const iterator<T, I, J> &other) const {
      return data_ - other.data_;
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    iterator<T, I, J>::reference_type iterator<T, I, J>::operator*() {
      return row_info<T, I, J>(*data_, *indices_, *cindices_);
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    iterator<T, I, J>::reference_type iterator<T, I, J>::operator[](
        const iterator<T, I, J>::difference_type &n) {
      return *(*this + n);
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    iterator<T, I, J>::reference_type iterator<T, I, J>::operator*() const {
      return row_info<T, I, J>(const_cast<T &>(*data_), const_cast<I &>(*indices_),
                               const_cast<J &>(*cindices_));
    }

    template <typename T, typename I, typename J>
      requires QMatrixTypes<T, I, J>
    iterator<T, I, J>::reference_type iterator<T, I, J>::operator[](
        const iterator<T, I, J>::difference_type &n) const {
      return *(*this + n);
    }

    template struct iterator<int8_t, int32_t, uint8_t>;
    template struct iterator<uint8_t, int32_t, uint8_t>;
    template struct iterator<int16_t, int32_t, uint8_t>;
    template struct iterator<uint16_t, int32_t, uint8_t>;
    template struct iterator<int32_t, int32_t, uint8_t>;
    template struct iterator<int64_t, int32_t, uint8_t>;
    template struct iterator<float, int32_t, uint8_t>;
    template struct iterator<double, int32_t, uint8_t>;
    template struct iterator<cfloat, int32_t, uint8_t>;
    template struct iterator<cdouble, int32_t, uint8_t>;

    template struct iterator<int8_t, int64_t, uint8_t>;
    template struct iterator<uint8_t, int64_t, uint8_t>;
    template struct iterator<int16_t, int64_t, uint8_t>;
    template struct iterator<uint16_t, int64_t, uint8_t>;
    template struct iterator<int32_t, int64_t, uint8_t>;
    template struct iterator<int64_t, int64_t, uint8_t>;
    template struct iterator<float, int64_t, uint8_t>;
    template struct iterator<double, int64_t, uint8_t>;
    template struct iterator<cfloat, int64_t, uint8_t>;
    template struct iterator<cdouble, int64_t, uint8_t>;

    template struct iterator<int8_t, int32_t, uint16_t>;
    template struct iterator<uint8_t, int32_t, uint16_t>;
    template struct iterator<int16_t, int32_t, uint16_t>;
    template struct iterator<uint16_t, int32_t, uint16_t>;
    template struct iterator<int32_t, int32_t, uint16_t>;
    template struct iterator<int64_t, int32_t, uint16_t>;
    template struct iterator<float, int32_t, uint16_t>;
    template struct iterator<double, int32_t, uint16_t>;
    template struct iterator<cfloat, int32_t, uint16_t>;
    template struct iterator<cdouble, int32_t, uint16_t>;

    template struct iterator<int8_t, int64_t, uint16_t>;
    template struct iterator<uint8_t, int64_t, uint16_t>;
    template struct iterator<int16_t, int64_t, uint16_t>;
    template struct iterator<uint16_t, int64_t, uint16_t>;
    template struct iterator<int32_t, int64_t, uint16_t>;
    template struct iterator<int64_t, int64_t, uint16_t>;
    template struct iterator<float, int64_t, uint16_t>;
    template struct iterator<double, int64_t, uint16_t>;
    template struct iterator<cfloat, int64_t, uint16_t>;
    template struct iterator<cdouble, int64_t, uint16_t>;

    template struct row_info<int8_t, int32_t, uint8_t>;
    template struct row_info<uint8_t, int32_t, uint8_t>;
    template struct row_info<int16_t, int32_t, uint8_t>;
    template struct row_info<uint16_t, int32_t, uint8_t>;
    template struct row_info<int32_t, int32_t, uint8_t>;
    template struct row_info<int64_t, int32_t, uint8_t>;
    template struct row_info<float, int32_t, uint8_t>;
    template struct row_info<double, int32_t, uint8_t>;
    template struct row_info<cfloat, int32_t, uint8_t>;
    template struct row_info<cdouble, int32_t, uint8_t>;

    template struct row_info<int8_t, int64_t, uint8_t>;
    template struct row_info<uint8_t, int64_t, uint8_t>;
    template struct row_info<int16_t, int64_t, uint8_t>;
    template struct row_info<uint16_t, int64_t, uint8_t>;
    template struct row_info<int32_t, int64_t, uint8_t>;
    template struct row_info<int64_t, int64_t, uint8_t>;
    template struct row_info<float, int64_t, uint8_t>;
    template struct row_info<double, int64_t, uint8_t>;
    template struct row_info<cfloat, int64_t, uint8_t>;
    template struct row_info<cdouble, int64_t, uint8_t>;

    template struct row_info<int8_t, int32_t, uint16_t>;
    template struct row_info<uint8_t, int32_t, uint16_t>;
    template struct row_info<int16_t, int32_t, uint16_t>;
    template struct row_info<uint16_t, int32_t, uint16_t>;
    template struct row_info<int32_t, int32_t, uint16_t>;
    template struct row_info<int64_t, int32_t, uint16_t>;
    template struct row_info<float, int32_t, uint16_t>;
    template struct row_info<double, int32_t, uint16_t>;
    template struct row_info<cfloat, int32_t, uint16_t>;
    template struct row_info<cdouble, int32_t, uint16_t>;

    template struct row_info<int8_t, int64_t, uint16_t>;
    template struct row_info<uint8_t, int64_t, uint16_t>;
    template struct row_info<int16_t, int64_t, uint16_t>;
    template struct row_info<uint16_t, int64_t, uint16_t>;
    template struct row_info<int32_t, int64_t, uint16_t>;
    template struct row_info<int64_t, int64_t, uint16_t>;
    template struct row_info<float, int64_t, uint16_t>;
    template struct row_info<double, int64_t, uint16_t>;
    template struct row_info<cfloat, int64_t, uint16_t>;
    template struct row_info<cdouble, int64_t, uint16_t>;

  }  // namespace details
}  // namespace quspin
