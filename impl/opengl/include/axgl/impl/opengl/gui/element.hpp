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
    should_render_ = false;

    const auto position = get_position(context);
    const auto model = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f))
      * glm::scale(glm::vec3(property_.size.x, property_.size.y, 1.0f));

    auto& shader = ::opengl::StaticShaders::instance().gui();
    shader.use_program();
    shader.set_vec4("color", glm::vec4(glm::vec3(property_.color), hovering_ ? 0.5f : 1.0f));
    shader.set_mat4("projection_view_model", context.projection * model);
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
