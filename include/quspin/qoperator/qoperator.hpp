#pragma once

#include <quspin/dtype/dtype.hpp>
#include <quspin/qoperator/details/qoperator.hpp>

namespace quspin {

  class Operator : public DTypeObject<details::qoperators> {
    using DTypeObject<details::qoperators>::internals_;

  public:
    Operator(const Operator &op);
    Operator(const details::qoperators &op);
    template <typename T, typename I, typename J> Operator(const details::qoperator<T, I, J> &op);
    Operator(Array, Array, Array, Array);
  };

}  // namespace quspin
