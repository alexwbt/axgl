#pragma once

#include <memory>

#include "axgl/namespace.hpp"
#include "axgl/interface/service.hpp"
#include "axgl/interface/window.hpp"

#include <glm/glm.hpp>

NAMESPACE_AXGL_INTERFACE

class Texture
{
public:
  static constexpr int kCubemapSize = 6;
  virtual ~Texture() {}
  virtual void load_2d_texture(std::span<const uint8_t> data) = 0;
  virtual void load_cubemap_texture(const std::array<std::span<const uint8_t>, kCubemapSize>& data) = 0;
};

class Renderer
{
public:
  virtual ~Renderer() {}
  virtual bool ready() = 0;
  virtual void before_render() = 0;
  virtual void after_render() = 0;
  virtual void set_window(std::shared_ptr<Window> window) = 0;
  virtual glm::ivec2 viewport() const = 0;

  virtual std::shared_ptr<Texture> create_texture() = 0;
};

class RendererService : public Service
{
public:
  virtual std::shared_ptr<Renderer> create_renderer() = 0;
};

NAMESPACE_AXGL_INTERFACE_END
