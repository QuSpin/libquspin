#pragma once

/*
 *  Functions to handle arithmetic operations on NumPy complex_wrapper values
 */

#include <complex>
#include <type_traits>

namespace quspin {
namespace details {

template <typename T> using complex_wrapper = std::complex<T>;

typedef std::complex<float> cfloat;
typedef std::complex<double> cdouble;
typedef std::complex<long double> cldouble;

// template<typename T, typename U, typename Op>
// std::common_type_t<T, U> operator_binary(const T &a,const U &b, Op &&op) {{
//     using arith_result = std::common_type_t<T, U>;
//     return op(static_cast<arith_result>(a), static_cast<arith_result>(b));
// }}

// template<typename T, typename U>
// std::common_type_t<T, U> operator+(const T &a, const U &b) {{
//     return operator_binary(a, b, [](auto a, auto b) {{ return a + b; }});
// }}

// template<typename T, typename U>
// std::common_type_t<T, U> operator-(const T &a, const U &b) {{
//     return operator_binary(a, b, [](auto a, auto b) {{ return a - b; }});
// }}

// template<typename T, typename U>
// std::common_type_t<T, U> operator*(const T &a, const U &b) {{
//     return operator_binary(a, b, [](auto a, auto b) {{ return a * b; }});
// }}

// template<typename T, typename U>
// std::common_type_t<T, U> operator/(const T &a, const U &b) {{
//     return operator_binary(a, b, [](auto a, auto b) {{ return a / b; }});
// }}

} // namespace details
} // namespace quspin