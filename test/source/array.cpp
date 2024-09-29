#include <doctest/doctest.h>
#include <quspin/array/array.hpp>
#include <quspin/dtype/dtype.hpp>
#include <quspin/version.hpp>

#include <string>

TEST_CASE("Array") {
  using namespace quspin;

  Array a({1, 2, 3}, Float);

}

TEST_CASE("QuSpin version") {
  static_assert(std::string_view(QUSPIN_VERSION) == std::string_view("0.1.0"));
  CHECK(std::string(QUSPIN_VERSION) == std::string("0.1.0"));
}
