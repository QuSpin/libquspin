#pragma once

namespace quspin {
namespace details {

template <typename Variant> class VariantContainer {
protected:
  Variant internals_;

  friend Variant get_variant_obj(const VariantContainer &);

public:
  VariantContainer() = default;
  VariantContainer(const Variant &internals) : internals_(internals) {}
  VariantContainer(const VariantContainer &obj) : internals_(obj.internals_) {}
};

} // namespace details

template <typename Variant>
Variant get_variant_obj(const details::VariantContainer<Variant> &obj) {
  return obj.internals_;
}

} // namespace quspin
