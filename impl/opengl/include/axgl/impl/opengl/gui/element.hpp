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
    should_render_ = false;

    const auto position = get_position(context);

    auto& shader = ::opengl::StaticShaders::instance().gui();
    shader.use_program();
    shader.set_vec4("color", property_.color);
    shader.set_mat4("projection_view_model", context.projection * get_model_matrix(position));
    ::opengl::StaticVAOs::instance().quad().draw();

    const auto height = static_cast<GLsizei>(property_.size.y);
    const auto screen_height = static_cast<GLint>(context.page.get_size().y);
    glScissor(
      static_cast<GLint>(position.x), screen_height - static_cast<GLint>(position.y) - height,
      static_cast<GLsizei>(property_.size.x), height);
    render_children(context);
  }
};

} // namespace axgl::impl::opengl::gui
