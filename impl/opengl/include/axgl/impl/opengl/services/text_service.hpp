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
#include <axgl/interface/services/text_service.hpp>

#include <axgl/impl/opengl/components/mesh.hpp>
#include <axgl/util/mesh.hpp>

#include <opengl/text.hpp>

namespace axgl::impl::opengl
{

class TextService : public axgl::TextService
{
  ::opengl::TextRenderer text_renderer_;

  axgl::ptr_t<axgl::RendererService> renderer_service_;

public:
  void initialize(const Service::Context& context) override { renderer_service_ = context.axgl.renderer_service(); }

  [[nodiscard]] bool has_font(const std::string& name) const { return text_renderer_.has_font(name); }

  void load_font(const std::string& name, const std::span<const std::uint8_t> data, const int index = 0) override
  {
#ifdef AXGL_DEBUG
    if (text_renderer_.has_font(name)) throw std::runtime_error("Font already exists: " + name);
#endif
    text_renderer_.load_font(name, data, index);
  }

  void unload_font(const std::string& name) override
  {
#ifdef AXGL_DEBUG
    if (!text_renderer_.has_font(name)) throw std::runtime_error("Font does not exist: " + name);
#endif
    text_renderer_.unload_font(name);
  }

  [[nodiscard]] axgl::ptr_t<axgl::Texture> create_texture(const Options& options) const override
  {
    ::opengl::Text text;
    text_renderer_.render_text(
      text, options.value, options.fonts,
      {
        .size = static_cast<std::uint32_t>(options.font_size),
        .color = options.font_color,
        .vertical = options.vertical,
      });

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
};

} // namespace axgl::impl::opengl

