#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/service.hpp>
#include <axgl/interface/texture.hpp>

namespace axgl
{

class TextService : public axgl::Service
{
public:
  static constexpr std::string_view kTypeId = "service::text";

  struct Options
  {
    const std::string& value;
    const std::vector<std::string>& fonts;
    const glm::vec4& font_color;
    float font_size = 16.0f;
    bool vertical = false;
  };

  virtual void load_font(const std::string& name, std::span<const std::uint8_t> data, int index) = 0;
  virtual void unload_font(const std::string& name) = 0;

  [[nodiscard]] virtual axgl::ptr_t<axgl::Texture> create_texture(const Options& options) const = 0;
};

} // namespace axgl
