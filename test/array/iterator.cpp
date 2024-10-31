
#include <iostream>
#include <quspin/array/details/array.hpp>

using namespace quspin::details;

int main() {
  array<float> arr(std::vector<std::size_t>({1000000}));

  //   int i = 0;
  for (auto it = arr.begin(); it != arr.end(); ++it) {
    auto dim_indices = it.dim_indices();
    std::cout << *it << " " << it.index_();
    // for (auto &dim_index : dim_indices) {
    //   std::cout << " " << dim_index;
    // }
    std::cout << std::endl;
    // i++;
    // if(i > 10000) break;
  }

  return 0;
}
