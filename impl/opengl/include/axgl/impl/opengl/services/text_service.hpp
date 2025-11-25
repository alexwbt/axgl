#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <vector>

#include <axgl/axgl.hpp>
#include <axgl/interface/components/mesh.hpp>
#include <axgl/interface/service.hpp>
#include <axgl/interface/services/entity_service.hpp>
#include <axgl/interface/services/renderer_service.hpp>

#include <axgl/impl/opengl/component/mesh.hpp>
#include <axgl/impl/service_base.hpp>

#include <opengl/static_vaos.hpp>
#include <opengl/text.hpp>

namespace axgl::impl
{

class OpenglTextService : public ServiceBase
{
  opengl::TextRenderer text_renderer_;

  std::shared_ptr<axgl::RendererService> renderer_service_;
  std::shared_ptr<axgl::EntityService> entity_service_;

public:
  void initialize() override
  {
    const auto context = get_context();
    renderer_service_ = context->axgl->renderer_service();
    entity_service_ = context->axgl->entity_service();
  }

  bool has_font(const std::string& name) const { return text_renderer_.has_font(name); }

  void load_font(const std::string& name, std::span<const uint8_t> data, int index = 0)
  {
#ifdef AXGL_DEBUG
    if (text_renderer_.has_font(name))
      throw std::runtime_error("Font already exists: " + name);
#endif
    text_renderer_.load_font(name, data, index);
  }

  void unload_font(const std::string& name)
  {
#ifdef AXGL_DEBUG
    if (!text_renderer_.has_font(name))
      throw std::runtime_error("Font does not exist: " + name);
#endif
    text_renderer_.unload_font(name);
  }

  std::shared_ptr<axgl::Entity> create_text(
    const std::string& value, const std::vector<std::string>& font, const opengl::TextOptions& options) const
  {
    opengl::Text text;
    text_renderer_.render_text(text, value, font, options);

    const auto texture = std::dynamic_pointer_cast<OpenglTexture>(renderer_service_->create_texture());
#ifdef AXGL_DEBUG
    if (!texture)
      throw std::runtime_error("OpenglTexture is required to use OpenglTextService");
#endif
    const auto texture_ptr = std::make_shared<opengl::Texture>();
    *texture_ptr = std::move(text.texture);
    texture->replace_texture(std::move(texture_ptr));

    const auto material = renderer_service_->create_material("2d");
    material->add_texture(axgl::Material::TextureType::kDiffuse, texture);
    material->set_enable_blend(true);

    const auto mesh = entity_service_->create_component_t<component::OpenglMesh>();
    mesh->replace_vao(opengl::StaticVAOs::instance().quad());
    mesh->set_material(material);

    const auto text_entity = entity_service_->create_entity();
    text_entity->transform()->scale = glm::vec3(text.size, 1.0f);
    text_entity->update_model_matrix();
    text_entity->components()->add(mesh);

    return text_entity;
  }
};

} // namespace axgl::impl

