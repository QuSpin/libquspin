
#include <iostream>
#include <quspin/details/threading.hpp>
#include <ranges>

int main() {
  std::vector<int> vec(10000);
  std::vector<int> out(vec.size());

  std::fill(vec.begin(), vec.end(), 10);

  auto range_iter = std::ranges::iota_view(std::size_t(), vec.size());

  quspin::details::async_for_each<100>(std::begin(range_iter), std::end(range_iter),
                                       [&out, &vec](auto &&i) {
                                         const auto ele = vec.at(i);
                                         out.at(i) = ele * ele;
                                       });

  return 0;
}
