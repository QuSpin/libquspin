#pragma once

#include <quspin/array/details/array.hpp>
#include <quspin/array/details/reference.hpp>
#include <quspin/array/details/scalar.hpp>

namespace quspin {
  namespace details {

    template <typename lhs_t, typename rhs_t>
    void check(const array<lhs_t> &lhs, const array<rhs_t> &rhs) {
      if (lhs.size() != rhs.size()) {
        throw std::runtime_error("Invalid array sizes");
      }

      if (lhs.ndim() != rhs.ndim()) {
        throw std::runtime_error("Invalid number of dimensions");
      }

      if (lhs.shape() != rhs.shape()) {
        throw std::runtime_error("Invalid array shapes");
      }
    }

    template <typename lhs_t, typename rhs_t>
    void check(const scalar<lhs_t> &, const array<rhs_t> &) {}

    template <typename lhs_t, typename rhs_t>
    void check(const array<lhs_t> &, const scalar<rhs_t> &) {}

    template <typename lhs_t, typename rhs_t>
    void check(const scalar<lhs_t> &, const scalar<rhs_t> &) {}

    template <typename lhs_t, typename rhs_t>
    void check(const reference<lhs_t> &, const array<rhs_t> &) {}

    template <typename lhs_t, typename rhs_t>
    void check(const array<lhs_t> &, const reference<rhs_t> &) {}

    template <typename lhs_t, typename rhs_t>
    void check(const reference<lhs_t> &, const reference<rhs_t> &) {}

    template <typename lhs_t, typename rhs_t>
    void check(const scalar<lhs_t> &, const reference<rhs_t> &) {}

    template <typename lhs_t, typename rhs_t>
    void check(const reference<lhs_t> &, const scalar<rhs_t> &) {}

  }  // namespace details
}  // namespace quspin
