#pragma once

#include <axgl/common.hpp>

#include <opengl/text.hpp>

NAMESPACE_AXGL_IMPL

class OpenglTextService : public interface::Service
{
private:
  opengl::FontCollection font_collection_;

public:
  bool has_font(const std::string& name) const
  {
    return font_collection_.has_font(name);
  }

  void load_font(const std::string& name, std::span<const uint8_t> data, int index = 0)
  {
#ifdef AXGL_DEBUG
    if (font_collection_.has_font(name))
      throw std::runtime_error("Font already exists: " + name);
#endif
    font_collection_.load_font(name, data, index);
  }

  void unload_font(const std::string& name)
  {
#ifdef AXGL_DEBUG
    if (!font_collection_.has_font(name))
      throw std::runtime_error("Font does not exist: " + name);
#endif
    font_collection_.unload_font(name);
  }

  std::shared_ptr<opengl::Texture> render_text(
    const std::string& value,
    const std::string& font,
    uint32_t size) const
  {
    auto texture = font_collection_.render_text(value, font, size);
    auto texture_ptr = std::make_shared<opengl::Texture>();
    *texture_ptr = std::move(texture);
    return texture_ptr;
  }
};

NAMESPACE_AXGL_IMPL_END
