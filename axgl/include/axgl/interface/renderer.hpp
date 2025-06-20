#pragma once

#include <span>
#include <memory>

#include <glm/glm.hpp>

#include <axgl/common.hpp>
#include <axgl/interface/service.hpp>
#include <axgl/interface/window.hpp>

NAMESPACE_AXGL_INTERFACE

class Texture
{
public:
  virtual ~Texture() = default;
  virtual void load_texture(std::span<const uint8_t> data) = 0;
};

enum class TextureType
{
  kUnknown,
  kDiffuse,
  kSpecular,
  kNormal,
  kHeight,
};

class Material
{
public:
  virtual ~Material() = default;
  virtual void set_color(const glm::vec4& color) = 0;
  virtual void set_prop(const std::string& key, const std::string& value) = 0;
  virtual void add_texture(TextureType type, std::shared_ptr<Texture> texture) = 0;
};

class Renderer
{
public:
  virtual ~Renderer() = default;
  virtual bool ready() = 0;
  virtual void before_render() = 0;
  virtual void after_render() = 0;
  virtual void set_window(std::shared_ptr<Window> window) = 0;
  [[nodiscard]] virtual glm::ivec2 viewport() const = 0;
};

class RendererService : public Service
{
public:
  virtual std::shared_ptr<Renderer> create_renderer() = 0;
  virtual std::shared_ptr<Texture> create_texture() = 0;
  virtual std::shared_ptr<Material> create_material(const std::string& type) = 0;
};

NAMESPACE_AXGL_INTERFACE_END
