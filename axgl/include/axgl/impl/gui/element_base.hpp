#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/gui/attribute.hpp>
#include <axgl/interface/gui/element.hpp>

#include <axgl/impl/gui/element_container.hpp>

namespace axgl::impl::gui
{

class ElementBase : virtual public axgl::gui::Element
{
protected:
  std::uint64_t id_ = 0;
  bool should_render_ = false;
  axgl::gui::Attribute attribute_;
  ElementContainer children_;

public:
  [[nodiscard]] std::uint64_t get_id() const override { return id_; }
  [[nodiscard]] bool should_render() const override { return should_render_ || children_.should_render(); }
  [[nodiscard]] axgl::gui::Attribute& attribute() override { return attribute_; }
  [[nodiscard]] axgl::Container<axgl::gui::Element>& children() override { return children_; }
};

} // namespace axgl::impl::gui
