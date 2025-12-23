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
  void render(const axgl::gui::Page::Context& context) override
  {
    const auto model = glm::translate(glm::mat4(1.0f), glm::vec3(attribute_.position, 0.0f)) *
                       glm::scale(glm::vec3(attribute_.size, 1.0f));

    auto& shader = ::opengl::StaticShaders::instance().gui();
    shader.use_program();
    shader.set_vec4("color", attribute_.color);
    shader.set_mat4("projection_view_model", context.projection * model);

    ::opengl::StaticVAOs::instance().quad().draw();

    axgl::gui::Page::Context current_context = context;
    current_context.parent = this;
    for (const auto& child : children_.get())
      child->render(current_context);
  }
};

} // namespace axgl::impl::opengl::gui
