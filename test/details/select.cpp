
#include <iostream>
#include <quspin/array/array.hpp>
#include <quspin/array/details/array.hpp>
#include <quspin/details/select.hpp>
#include <quspin/dtype/dtype.hpp>

int main() {
  using namespace quspin;

  Array a({10}, Int8);

  auto b = select<details::array<int8_t>>(a);

  try {
    auto c = select<details::array<int16_t>>(a);
  } catch (const std::invalid_argument &e) {
    std::cout << e.what() << std::endl;
  }

  return 0;
}
