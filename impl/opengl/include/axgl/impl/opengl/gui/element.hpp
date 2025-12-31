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
  void on_pointer_enter(const axgl::gui::Page::Context& context) override
  {
    axgl::impl::gui::ElementBase::on_pointer_enter(context);
  }

  void render(const axgl::gui::Page::RenderContext& context) override
  {
    const auto model                                                //
      = glm::translate(glm::mat4(1.0f), glm::vec3(position_, 0.0f)) //
      * glm::scale(glm::vec3(size_.x, size_.y, 1.0f));              //

    const auto& style = current_style();
    auto& shader = ::opengl::StaticShaders::instance().gui();
    shader.use_program();
    shader.set_vec4("color", style->get_color());
    shader.set_float("opacity", style->get_opacity());
    shader.set_mat4("projection_view_model", context.projection * model);
    ::opengl::StaticVAOs::instance().quad().draw();

    glScissor(
      static_cast<GLint>(scissor_rect_.x),                                           //
      static_cast<GLint>(context.page_height) - static_cast<GLint>(scissor_rect_.w), //
      static_cast<GLsizei>(scissor_rect_.z - scissor_rect_.x),                       //
      static_cast<GLsizei>(scissor_rect_.w - scissor_rect_.y));                      //

    render_children(context);
  }
};

} // namespace axgl::impl::opengl::gui
