#include <doctest/doctest.h>

#include <quspin/array/array.hpp>
#include <quspin/array/kernel/kernels.hpp>
#include <quspin/dtype/dtype.hpp>
#include <quspin/version.hpp>
#include <string>


TEST_CASE("Array Set and Get") {
  using namespace quspin;



  Array a({1, 2, 3}, Float);
  CHECK(a.ndim() == 3);
  CHECK(a.size() == 6);
  std::vector<std::size_t> shape = a.shape();
  CHECK(shape == std::vector<std::size_t>({1, 2, 3}));

  for (std::size_t i = 0; i < a.shape(0); i++) {
    for (std::size_t j = 0; j < a.shape(1); j++) {
      for (std::size_t k = 0; k < a.shape(2); k++) {
        a[{i, j, k}] = static_cast<float>(i + j + k);
      }
    }
  }

  for (std::size_t i = 0; i < a.shape(0); i++) {
    for (std::size_t j = 0; j < a.shape(1); j++) {
      for (std::size_t k = 0; k < a.shape(2); k++) {
        CHECK(static_cast<float>(a[{i, j, k}]) == static_cast<float>(i + j + k));
      }
    }
  }
}

TEST_CASE("Array Sum"){
  using namespace quspin;

  Array a({10}, Int64);
  Array b({10}, Int32);
  Array c({10}, Int64);

  for (std::size_t i = 0; i < a.shape(0); i++) {
    a[{i}] = static_cast<int64_t>(i);
    b[{i}] = static_cast<int32_t>(2*i);
  }

  Array d = add(a, b);

  CHECK(d.dtype() == Int64);

  for (std::size_t i = 0; i < a.shape(0); i++) {
    CHECK(static_cast<int64_t>(d[{i}]) == static_cast<int64_t>(3*i));
  }
}

TEST_CASE("QuSpin version") {
  static_assert(std::string_view(QUSPIN_VERSION) == std::string_view("0.1.0"));
  CHECK(std::string(QUSPIN_VERSION) == std::string("0.1.0"));
}
