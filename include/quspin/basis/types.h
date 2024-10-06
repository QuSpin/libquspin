#ifndef __QUSPIN_BASIS_BITBASIS_TYPES_H__
#define __QUSPIN_BASIS_BITBASIS_TYPES_H__

#include <cinttypes>

#ifdef USE_BOOST
#include "boost/multiprecision/cpp_int.hpp"
#include "boost/numeric/conversion/cast.hpp"
#endif

namespace quspin::basis {

typedef std::uint8_t  uint8_t;
typedef std::uint16_t uint16_t;
typedef std::uint32_t uint32_t;
typedef std::uint64_t uint64_t;

typedef int dit_integer_t; 

#ifdef USE_BOOST
typedef boost::multiprecision::uint128_t uint128_t;
typedef boost::multiprecision::uint256_t uint256_t;
typedef boost::multiprecision::uint512_t uint512_t;
typedef boost::multiprecision::uint1024_t uint1024_t;
typedef boost::multiprecision::number<boost::multiprecision::cpp_int_backend<2048UL, 2048UL, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void> > uint2048_t;
typedef boost::multiprecision::number<boost::multiprecision::cpp_int_backend<4096UL, 4096UL, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void> > uint4096_t;
typedef boost::multiprecision::number<boost::multiprecision::cpp_int_backend<8192UL, 8192UL, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void> > uint8192_t;
typedef boost::multiprecision::number<boost::multiprecision::cpp_int_backend<16384UL, 16384UL, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void> > uint16384_t;
#endif

}
#endif