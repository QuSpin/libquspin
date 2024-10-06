#include <cassert>
#include <string>
#include <exception>


#include <quspin/array/array.hpp>
#include <quspin/array/kernel/kernels.hpp>
#include <quspin/dtype/dtype.hpp>

int main() {
  using namespace quspin;

  Array a({1, 2, 3}, Float);
  assert(a.ndim() == 3);
  assert(a.size() == 6);
  std::vector<std::size_t> shape = a.shape();
  assert(shape == std::vector<std::size_t>({1, 2, 3}));

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
        assert(static_cast<float>(a[{i, j, k}]) == static_cast<float>(i + j + k));
      }
    }
  }

  Array a({100}, Int64);
  Array b({100}, Float);

  for (std::size_t i = 0; i < a.shape(0); i++) {
    a[{i}] = i;
    b[{i}] = 2 * i;
  }

  Array d = add(a, b);

  assert(d.dtype() == Float);

  for (std::size_t i = 0; i < a.shape(0); i++) {
    assert(static_cast<float>(d[{i}]) == static_cast<float>(3 * i));
  }


  Array a({10}, Int64);
  Array b({9}, Float);
  Array c({10}, Int64);
  Array out({10}, Int8);

  try {
    add(a, b);
  } catch (const std::invalid_argument &e) {
    assert(std::string(e.what()) == std::string("Incompatible shapes"));
  }

  try {
    add(a, c, out);
  } catch (const std::invalid_argument &e) {
    assert(std::string(e.what()) == std::string("Incompatible out type"));
  }
  

}


