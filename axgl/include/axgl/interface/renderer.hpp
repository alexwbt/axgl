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
  enum Type
  {
    UNKNOWN,
    DIFFUSE,
    SPECULAR,
    NORMAL,
    HEIGHT,
  };
  Type type;
  virtual ~Texture() {}
  virtual void load_texture(Type type, std::span<const uint8_t> data) = 0;
};

class Material
{
public:
  virtual ~Material() {}
  virtual void set_color(const glm::vec3& color) = 0;
  virtual void add_texture(std::shared_ptr<Texture> texture) = 0;
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
};

class RendererService : public Service
{
public:
  virtual std::shared_ptr<Renderer> create_renderer() = 0;
  virtual std::shared_ptr<Texture> create_texture() = 0;
  virtual std::shared_ptr<Material> create_material(const std::string& type) = 0;
};

NAMESPACE_AXGL_INTERFACE_END
