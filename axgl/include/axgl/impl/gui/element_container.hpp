#pragma once

#include <algorithm>
#include <vector>

#include <axgl/common.hpp>
#include <axgl/interface/container.hpp>

#include <axgl/interface/gui/element.hpp>

namespace axgl::impl::gui
{

class ElementContainer : virtual public axgl::Container<axgl::gui::Element>
{
  std::vector<axgl::ptr_t<axgl::gui::Element>> children_;

public:
  void add(axgl::ptr_t<axgl::gui::Element> element) override { children_.push_back(std::move(element)); }
  void remove(const axgl::ptr_t<axgl::gui::Element>& element) override { std::erase(children_, element); }
  [[nodiscard]] std::span<const ptr_t<axgl::gui::Element>> get() const override { return children_; }

  [[nodiscard]] bool should_render() const
  {
    return std::ranges::any_of(children_, [](const auto& e) { return e->should_render(); });
  }
};

} // namespace axgl::impl::gui
