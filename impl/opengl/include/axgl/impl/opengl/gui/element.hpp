#pragma once

#include <glm/gtx/transform.hpp>

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
  void render(const axgl::gui::Page::RenderContext& context) override
  {
    const auto position = get_position(context);
    const auto size = get_size(context);
    const auto model
      = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f)) * glm::scale(glm::vec3(size.x, size.y, 1.0f));

    auto& shader = ::opengl::StaticShaders::instance().gui();
    shader.use_program();
    shader.set_vec4("color", property_.color);
    shader.set_mat4("projection_view_model", context.projection * model);
    ::opengl::StaticVAOs::instance().quad().draw();

    glScissor(
      static_cast<GLint>(position.x),
      context.page.get_size().y - static_cast<GLint>(position.y) - static_cast<GLint>(size.y),
      static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y));
    render_children(context);
  }
};

} // namespace axgl::impl::opengl::gui
