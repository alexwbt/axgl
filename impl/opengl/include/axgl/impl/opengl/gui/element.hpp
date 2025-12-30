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
    const auto model                                               //
      = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f)) //
      * glm::scale(glm::vec3(size.x, size.y, 1.0f));               //

    const auto& style = current_style();
    auto& shader = ::opengl::StaticShaders::instance().gui();
    shader.use_program();
    shader.set_vec4("color", style->get_color());
    shader.set_float("opacity", style->get_opacity());
    shader.set_mat4("projection_view_model", context.projection * model);
    ::opengl::StaticVAOs::instance().quad().draw();

    glm::vec4 scissor_rect = {position, position + size};
    if (context.parent_rect)
    {
      scissor_rect.x = std::max(scissor_rect.x, context.parent_rect->x);
      scissor_rect.y = std::max(scissor_rect.y, context.parent_rect->y);
      scissor_rect.z = std::min(scissor_rect.z, context.parent_rect->z);
      scissor_rect.w = std::min(scissor_rect.w, context.parent_rect->w);
    }
    glScissor(
      static_cast<GLint>(scissor_rect.x),                             //
      context.page.get_size().y - static_cast<GLint>(scissor_rect.w), //
      static_cast<GLsizei>(scissor_rect.z - scissor_rect.x),          //
      static_cast<GLsizei>(scissor_rect.w - scissor_rect.y));         //

    render_children(context, &scissor_rect);
  }
};

} // namespace axgl::impl::opengl::gui
