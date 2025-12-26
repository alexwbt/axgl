#pragma once

#include <algorithm>

#include <axgl/interface/gui/page.hpp>

#include <axgl/impl/gui/element_container.hpp>

namespace axgl::impl::gui
{

class PageBase : virtual public axgl::gui::Page
{
protected:
  std::uint32_t width_ = 0;
  std::uint32_t height_ = 0;
  axgl::impl::gui::ElementContainer elements_;

public:
  void set_size(std::uint32_t width, std::uint32_t height) override
  {
    width_ = width;
    height_ = height;
  }

  void init(const axgl::Service::Context& context) override { }

  void update(const axgl::Service::Context& context) override { }

  [[nodiscard]] bool should_render() const override
  {
    return std::ranges::any_of(elements_.get(), [](const auto& e) { return e->should_render(); });
  }

  [[nodiscard]] glm::ivec2 get_size() const override { return {width_, height_}; }

  [[nodiscard]] axgl::Container<axgl::gui::Element>& elements() override { return elements_; }
};

} // namespace axgl::impl::gui
