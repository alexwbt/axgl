#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <axgl/axgl.hpp>
#include <axgl/interface/components/mesh.hpp>
#include <axgl/interface/service.hpp>
#include <axgl/interface/services/entity_service.hpp>
#include <axgl/interface/services/renderer_service.hpp>
#include <axgl/interface/texture.hpp>

#include <axgl/impl/opengl/components/mesh.hpp>
#include <axgl/util/mesh.hpp>

#include <opengl/text.hpp>

namespace axgl::impl::opengl
{

class TextService : public axgl::Service
{
public:
  static constexpr std::string_view kTypeId = "service::text";

private:
  ::opengl::TextRenderer text_renderer_;

  axgl::ptr_t<axgl::RendererService> renderer_service_;
  axgl::ptr_t<axgl::EntityService> entity_service_;

public:
  void initialize(const Service::Context& context) override
  {
    renderer_service_ = context.axgl.renderer_service();
    entity_service_ = context.axgl.entity_service();
  }

  [[nodiscard]] bool has_font(const std::string& name) const { return text_renderer_.has_font(name); }

  void load_font(const std::string& name, const std::span<const uint8_t> data, const int index = 0)
  {
#ifdef AXGL_DEBUG
    if (text_renderer_.has_font(name)) throw std::runtime_error("Font already exists: " + name);
#endif
    text_renderer_.load_font(name, data, index);
  }

  void unload_font(const std::string& name)
  {
#ifdef AXGL_DEBUG
    if (!text_renderer_.has_font(name)) throw std::runtime_error("Font does not exist: " + name);
#endif
    text_renderer_.unload_font(name);
  }

  [[nodiscard]] axgl::ptr_t<axgl::impl::opengl::Texture> create_texture(
    const std::string& value,
    const std::vector<std::string>& font,
    const ::opengl::TextOptions& options,
    ::opengl::Text& text) const
  {
    text_renderer_.render_text(text, value, font, options);

    const auto texture = std::dynamic_pointer_cast<axgl::impl::opengl::Texture>(renderer_service_->create_texture());
#ifdef AXGL_DEBUG
    if (!texture)
      throw std::runtime_error("axgl::impl::opengl::Texture is required to use axgl::impl::opengl::TextService");
#endif
    auto texture_ptr = axgl::create_ptr<::opengl::Texture>();
    *texture_ptr = std::move(text.texture);
    texture->replace_texture(std::move(texture_ptr));

    return texture;
  }

  [[nodiscard]] axgl::ptr_t<axgl::Material> create_material(
    const std::string& value,
    const std::vector<std::string>& font,
    const ::opengl::TextOptions& options,
    ::opengl::Text& text) const
  {
    const auto texture = create_texture(value, font, options, text);
    const auto material = renderer_service_->create_material("2d");
    material->add_texture(axgl::Material::TextureType::kDiffuse, texture);
    return material;
  }

  [[nodiscard]] axgl::ptr_t<axgl::component::Mesh> create_mesh(
    const std::string& value,
    const std::vector<std::string>& font,
    const ::opengl::TextOptions& options,
    ::opengl::Text& text) const
  {
    const auto material = create_material(value, font, options, text);
    const auto mesh = entity_service_->create_component_t<impl::opengl::component::Mesh>();
    axgl::util::init_quad(*mesh);
    mesh->set_material(material);
    return mesh;
  }

  [[nodiscard]] axgl::ptr_t<axgl::Entity> create_entity(
    const std::string& value,
    const std::vector<std::string>& font,
    const ::opengl::TextOptions& options,
    const float scale = 1.0f) const
  {
    ::opengl::Text text;
    const auto mesh = create_mesh(value, font, options, text);
    const auto entity = entity_service_->create_entity();
    entity->components().add(mesh);
    entity->transform().scale = glm::vec3(text.size, 1.0f) * scale;
    entity->update_model_matrix();
    return entity;
  }
};

} // namespace axgl::impl::opengl

