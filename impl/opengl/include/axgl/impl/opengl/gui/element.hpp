#pragma once

#include <glm/gtx/transform.hpp>

#include <axgl/interface/gui/element.hpp>
#include <axgl/interface/gui/page.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/gui/element_base.hpp>
#include <axgl/impl/opengl/shaders.hpp>
#include <axgl/impl/opengl/texture.hpp>

#include <opengl/static_vaos.hpp>

namespace axgl::impl::opengl::gui {

class Element : public axgl::impl::gui::ElementBase {
public:
  void render(const axgl::gui::Context& context) override {
    render_base(context);
    render_children(context);
  }

protected:
  void render_base(const axgl::gui::Context& context) {
    const auto scissor_x = util::clamp_cast<GLint>(scissor_rect_.x);
    const auto scissor_y = util::clamp_cast<GLint>(context.page->get_height())
      - util::clamp_cast<GLint>(scissor_rect_.w);
    const auto scissor_width
      = util::clamp_cast<GLsizei>(scissor_rect_.z - scissor_rect_.x);
    const auto scissor_height
      = util::clamp_cast<GLsizei>(scissor_rect_.w - scissor_rect_.y);

    if (scissor_width <= 0 || scissor_height <= 0) {
      glScissor(scissor_x, scissor_y, 0, 0);
      return;
    }

    glScissor(scissor_x, scissor_y, scissor_width, scissor_height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    draw_quad(
      context,
      position_,
      size_,
      computed_style_->get_color(),
      computed_style_->get_opacity()
    );
  }

  void draw_quad(
    const axgl::gui::Context& context,
    const glm::vec2& position,
    const glm::vec2& size,
    const glm::vec4& color,
    float opacity
  ) {
    const auto model
      = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f))
      * glm::scale(glm::vec3(size.x, size.y, 1.0f));
    const auto& shader = Shaders::instance().gui();
    shader.use_program();
    shader.set_bool("use_texture", false);
    shader.set_vec4("color", color);
    shader.set_float("opacity", opacity);
    shader.set_mat4("projection_view_model", context.projection * model);
    ::opengl::StaticVAOs::instance().quad().draw();
  }
};

} // namespace axgl::impl::opengl::gui
