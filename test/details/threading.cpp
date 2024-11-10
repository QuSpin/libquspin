
#include <iostream>
#include <quspin/details/threading.hpp>
#include <ranges>

int main() {
  std::vector<int> vec = std::ranges::iota_view(1, 10000) | std::ranges::to<std::vector<int>>();
  std::vector<int> out(vec.size());

  out.reserve(vec.size());

  quspin::details::async_for_each<100>(vec.begin(), vec.end(),
                                       [&out](int &i) { out.at(i - 1) = i + 1; });

  return 0;
}
