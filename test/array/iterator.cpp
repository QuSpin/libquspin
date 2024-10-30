
#include <iostream>
#include <quspin/array/details/array.hpp>

using namespace quspin::details;

int main() {
  std::vector<std::size_t> shape = {100000};
  array<float> arr(shape);

  for (auto it = arr.begin(); it != arr.end(); ++it) {
    std::cout << *it << " " << it.index_() << std::endl;
  }

  return 0;
}
