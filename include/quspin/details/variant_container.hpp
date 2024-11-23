// Copyright 2024 Phillip Weinberg
#pragma once

namespace quspin {
namespace details {

template <typename Variant>
class VariantContainer {
 protected:
  Variant internals_;

 public:
  VariantContainer() = default;
  VariantContainer(const Variant &internals) : internals_(internals) {}
  Variant get_variant_obj() const { return internals_; }
};

}  // namespace details

}  // namespace quspin
