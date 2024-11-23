
#include <cassert>
#include <exception>
#include <quspin/array/array.hpp>
#include <quspin/dtype/dtype.hpp>
#include <quspin/qmatrix/qmatrix.hpp>
#include <string>

using namespace quspin;

int main() {
  int32_t indptr_ptr[4] = {0, 2, 4, 6};
  int32_t indices_ptr[6] = {0, 1, 1, 2, 0, 2};
  uint8_t cindices_ptr[6] = {0, 1, 1, 1, 0, 0};
  int8_t cindices_ptr2[6] = {0, 1, 1, 1, 0, 0};

  int8_t data_ptr[6] = {1, 2, 3, -3, -2, -1};

  Array indptr({4}, {sizeof(int32_t)}, Int32,
               reinterpret_cast<void *>(indptr_ptr));
  Array indices({6}, {sizeof(int32_t)}, Int32,
                reinterpret_cast<void *>(indices_ptr));
  Array cindices({6}, {sizeof(int8_t)}, UInt8,
                 reinterpret_cast<void *>(cindices_ptr));
  Array cindices2({6}, {sizeof(int8_t)}, Int8,
                  reinterpret_cast<void *>(cindices_ptr2));
  Array data({6}, {sizeof(int8_t)}, Int8, reinterpret_cast<void *>(data_ptr));

  QMatrix op(data, indptr, indices, cindices);
  QMatrix op2(data, indptr, indices, static_cast<uint8_t>(0));

  try {
    QMatrix op(data, indptr, indices, cindices2);
  } catch (const std::invalid_argument &e) {
    std::cout << e.what() << std::endl;
  }

  return 0;
}
