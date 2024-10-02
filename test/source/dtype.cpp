#include <quspin/dtype/dtype.hpp>
#include <doctest/doctest.h>

TEST_CASE("DTYPES") {
  using namespace quspin;

  CHECK(Int8 == DType::of<int8_t>());
  CHECK(Int16 == DType::of<int16_t>());
  CHECK(Int32 == DType::of<int32_t>());
  CHECK(Int64 == DType::of<int64_t>());
  CHECK(UInt8 == DType::of<uint8_t>());
  CHECK(UInt16 == DType::of<uint16_t>());
  CHECK(UInt32 == DType::of<uint32_t>());
  CHECK(UInt64 == DType::of<uint64_t>());
  CHECK(Float == DType::of<float>());
  CHECK(Double == DType::of<double>());
  CHECK(CFloat == DType::of<details::cfloat>());
  CHECK(CDouble == DType::of<details::cdouble>());
  
}