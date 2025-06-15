#pragma once

#include <span>
#include <string>
#include <vector>
#include <memory>
#include <stdint.h>

#include <axgl/common.hpp>
#include <axgl/interface/service.hpp>
#include <axgl/interface/renderer.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/impl/opengl/component/mesh.hpp>

#include <opengl/text.hpp>
#include <opengl/static_vaos.hpp>

NAMESPACE_AXGL_IMPL

struct OpenglText final
{
  std::shared_ptr<OpenglTexture> texture;
  glm::vec2 offset{ 0 };
};

class OpenglTextService : public interface::Service
{
private:
  opengl::TextRenderer text_renderer_;

  std::shared_ptr<interface::RendererService> renderer_service_;
  std::shared_ptr<interface::RealmService> realm_service_;

public:
  void initialize() override
  {
    auto context = get_context();
    renderer_service_ = context->axgl->renderer_service();
    realm_service_ = context->axgl->realm_service();
  }

  bool has_font(const std::string& name) const
  {
    return text_renderer_.has_font(name);
  }

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

  std::shared_ptr<interface::component::Mesh> create_text(
    const std::string& value,
    const std::vector<std::string>& font,
    opengl::TextOptions options
  ) const
  {
    opengl::Text text;
    text_renderer_.render_text(text, value, font, options);

    auto texture = std::dynamic_pointer_cast<OpenglTexture>(renderer_service_->create_texture());
#ifdef AXGL_DEBUG
    if (!texture)
      throw std::runtime_error("OpenglTexture is required to use OpenglTextService");
#endif
    auto texture_ptr = std::make_shared<opengl::Texture>();
    *texture_ptr = std::move(text.texture);
    texture->replace_texture(std::move(texture_ptr));

    auto material = renderer_service_->create_material("2d");
    material->add_texture(interface::TextureType::kDiffuse, texture);

    auto mesh = realm_service_->create_component_impl<interface::component::Mesh, component::OpenglMesh>();
    mesh->replace_vao(opengl::StaticVAOs::instance().quad());
    mesh->set_material(material);

    return mesh;
  }
};

NAMESPACE_AXGL_IMPL_END
