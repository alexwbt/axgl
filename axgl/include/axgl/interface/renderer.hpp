#pragma once

#include <memory>

#include <glm/glm.hpp>

#include <axgl/interface/window.hpp>

namespace axgl
{

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

} // namespace axgl
