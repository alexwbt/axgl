#pragma once

#include <axgl/interface/gui/element.hpp>
#include <axgl/interface/gui/page.hpp>

#include <axgl/impl/gui/element_base.hpp>

#include <opengl/static_shaders.hpp>
#include <opengl/static_vaos.hpp>

namespace axgl::impl::opengl::gui
{

class Element : virtual public axgl::gui::Element, public axgl::impl::gui::ElementBase
{
public:
  void render(const axgl::gui::Page::Context& context) override
  {
    auto& shader = ::opengl::StaticShaders::instance().gui();
    shader.use_program();
    shader.set_vec2("position", attribute_.position);
    shader.set_vec2("size", attribute_.size);
    shader.set_vec4("margin", attribute_.margin);
    shader.set_vec4("padding", attribute_.padding);
    shader.set_vec4("color", attribute_.color);
    shader.set_vec4("border_color", attribute_.border_color);
    shader.set_float("border_width", attribute_.border_width);
    shader.set_float("border_radius", attribute_.border_radius);

    ::opengl::StaticVAOs::instance().quad().draw();

    const axgl::gui::Page::Context current_context{context.axgl, context.gui_service, context.page, this};
    for (const auto& child : children_.get())
      child->render(current_context);
  }
};

} // namespace axgl::impl::opengl::gui
