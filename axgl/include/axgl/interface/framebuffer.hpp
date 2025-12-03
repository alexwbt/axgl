#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/texture.hpp>

namespace axgl
{

class Framebuffer
{
public:
  virtual ~Framebuffer() = default;
  virtual void use(std::span<const std::uint32_t> targets) = 0;
  virtual std::uint32_t attach_texture(axgl::ptr_t<axgl::Texture> texture) = 0;
};

} // namespace axgl
