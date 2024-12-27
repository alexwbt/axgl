#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/component/mesh.hpp>
#include <axgl/impl/realm_service.hpp>
#include <axgl/impl/opengl/text.hpp>

NAMESPACE_AXGL_IMPL

class OpenglGuiElement : public interface::GuiElement
{
private:
  impl::Component comp_impl_;

  std::shared_ptr<OpenglTextService> text_service_;
  std::shared_ptr<opengl::Texture> content_text_texture_;

public:
  void on_create() override
  {
    auto context = get_context();
    text_service_ = context->axgl->get_service<OpenglTextService>("text");

    content_text_texture_ = text_service_->render_text(props.content, props.font, props.font_size);
  }

  void add_component(std::shared_ptr<interface::Component> component) override
  {
    auto element = std::dynamic_pointer_cast<interface::GuiElement>(component);
#ifdef AXGL_DEBUG
    if (!element)
      throw std::runtime_error(
        "Invalid child component type. "
        "Children of GuiElement must also be GuiElement.");
#endif
    comp_impl_.add_component(std::move(component));
  }

  void remove_component(uint32_t id) override
  {
    comp_impl_.remove_component(id);
  }

  util::Iterable<std::shared_ptr<interface::Component>> get_components() const override
  {
    return comp_impl_.get_components();
  }

  void add_child(std::shared_ptr<interface::GuiElement> element) override
  {
    add_component(std::move(element));
  }

  void remove_child(uint32_t id) override
  {
    remove_component(id);
  }

  util::Iterable<std::shared_ptr<interface::GuiElement>> get_children() const override
  {
    throw std::runtime_error("Not implemented");
    // const auto& children = comp_impl_.get_component_vector();
    // return util::to_iterable(children);
  }

  void update() override
  {
    comp_impl_.update();
  }

  void render() override
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto& shader = opengl::StaticShaders::instance().mesh_2d();
    shader.use_program();
    //shader.set_mat4("mvp", mat);

    glActiveTexture(GL_TEXTURE0);
    content_text_texture_->use();
    shader.set_int("mesh_texture", 0);
    shader.set_bool("use_texture", true);

    opengl::StaticVAOs::instance().quad().draw();

    glDisable(GL_BLEND);

    comp_impl_.render();
  }

  uint32_t down_tick() { return 0; }
  bool hovering() { return false; }
  bool focused() { return false; }
};

NAMESPACE_AXGL_IMPL_END
