#pragma once

#ifdef AXGL_DEBUG
#include <stdexcept>
#endif
#include <memory>

#include <axgl/interface/gui/element.hpp>

#include <axgl/impl/gui/element_container.hpp>

namespace axgl::impl::gui
{

class ElementBase : public virtual axgl::gui::Element
{
  std::weak_ptr<Element> parent_;
  ElementContainer children_;

public:
  ElementBase() : children_(this) { }

  [[nodiscard]] ElementContainer* children() override { return &children_; }

  void set_parent(std::weak_ptr<Element> parent) override { parent_ = std::move(parent); }
  [[nodiscard]] std::shared_ptr<Element> get_parent() override
  {
#ifdef AXGL_DEBUG
    if (!parent_.expired())
      throw std::runtime_error("Component entity is not set.");
#endif
    return parent_.lock();
  }
};

} // namespace axgl::impl::gui
