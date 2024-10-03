#pragma once

/*
 *  Functions to handle arithmetic operations on NumPy complex_wrapper values
 */

#include <quspin/details/cast.hpp>

namespace quspin {
  namespace details {

    template <typename T, typename U, typename Op>
    upcast_t<T, U> operator_binary(const T &a, const U &b, Op &&op) {
      {
        using arith_result = upcast_t<T, U>;
#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4244)  // disable warning for implicit conversion
#endif
        return op(static_cast<arith_result>(a), static_cast<arith_result>(b));
#ifdef _MSC_VER
#  pragma warning(pop)
#endif
      }
    }

  }  // namespace details
}  // namespace quspin

template <typename T, typename U>
quspin::details::upcast_t<T, U> operator+(const T &a, const U &b) {
  {
    return quspin::details::operator_binary(a, b, [](auto &&a, auto &&b) {
      { return a + b; }
    });
  }
}

template <typename T, typename U>
quspin::details::upcast_t<T, U> operator-(const T &a, const U &b) {
  {
    return quspin::details::operator_binary(a, b, [](auto &&a, auto &&b) {
      { return a - b; }
    });
  }
}

template <typename T, typename U>
quspin::details::upcast_t<T, U> operator*(const T &a, const U &b) {
  {
    return quspin::details::operator_binary(a, b, [](auto &&a, auto &&b) {
      { return a * b; }
    });
  }
}

template <typename T, typename U>
quspin::details::upcast_t<T, U> operator/(const T &a, const U &b) {
  {
    return quspin::details::operator_binary(a, b, [](auto &&a, auto &&b) {
      { return a / b; }
    });
  }
}