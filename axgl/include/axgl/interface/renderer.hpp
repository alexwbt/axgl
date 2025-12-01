#pragma once

#include <cstdint>
#include <memory>

#include <glm/glm.hpp>

#include <axgl/interface/window.hpp>

namespace axgl
{

class Renderer
{
public:
  enum class Stage
  {
    kDefault,
    kBlend,
  };

  struct Renderable
  {
    Stage stage;
    std::int32_t sorted;
    virtual ~Renderable() = default;
    virtual void render() const = 0;
  };

  virtual ~Renderer() = default;
  virtual bool ready() = 0;
  virtual void render() = 0;
  virtual void set_window(std::shared_ptr<Window> window) = 0;
  [[nodiscard]] virtual glm::ivec2 viewport() const = 0;

  virtual void add_renderable(const Renderable* renderable) = 0;
  virtual void remove_renderable(const Renderable* renderable) = 0;
};

} // namespace axgl
