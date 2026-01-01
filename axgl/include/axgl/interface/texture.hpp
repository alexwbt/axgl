#pragma once

#include <cstdint>
#include <span>

namespace axgl
{

class Texture
{
public:
  virtual ~Texture() = default;
  virtual void load_texture(std::span<const std::uint8_t> data) = 0;
  [[nodiscard]] virtual std::uint32_t get_width() = 0;
  [[nodiscard]] virtual std::uint32_t get_height() = 0;
};

} // namespace axgl
