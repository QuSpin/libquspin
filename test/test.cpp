// Copyright 2024 Phillip Weinberg

#include <quspin/details/type_concepts.hpp>

int main() {
  static_assert(quspin::PrimativeTypes<const std::complex<float>>);

  const int arr[5] = {1, 2, 3, 4, 5};
}
