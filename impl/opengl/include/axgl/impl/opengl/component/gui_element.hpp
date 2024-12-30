#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <axgl/common.hpp>
#include <axgl/interface/component/mesh.hpp>
#include <axgl/impl/opengl/text.hpp>
#include <axgl/util/string.hpp>

NAMESPACE_AXGL_IMPL

class OpenglGuiElement : public interface::GuiElement
{
private:
  impl::Component comp_impl_;
  std::shared_ptr<interface::Mesh> content_text_;

public:
  void on_create() override
  {
    if (!props.content.empty())
    {
      auto context = get_context();
      auto text_service = context->axgl->get_service<OpenglTextService>("text");

      auto font = util::split(props.font, ',');
      for (const auto& f : font)
      {
        if (!text_service->has_font(f))
        {
          auto resource_service = context->axgl->resource_service();
          auto resource_key = "font/" + f + ".ttf";
#ifdef AXGL_DEBUG
          if (!resource_service->has_resource(resource_key))
            throw std::runtime_error("Required font not found: " + resource_key);
#endif
          text_service->load_font(f, resource_service->get_resource(resource_key));
        }
      }

      opengl::TextOptions options{ .size = static_cast<uint32_t>(props.font_size) };
      content_text_ = text_service->create_text(props.content, font, options);

      comp_impl_.add_component(content_text_);
    }
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
  }

  void update() override
  {
    comp_impl_.update();
  }

  void render() override
  {
    comp_impl_.render();
  }

  uint32_t down_tick() override { return 0; }
  bool hovering() override { return false; }
  bool focused() override { return false; }
};

NAMESPACE_AXGL_IMPL_END
