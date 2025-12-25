#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/container.hpp>
#include <axgl/interface/service.hpp>

namespace axgl
{
class Axgl;
class Texture;
class GuiService;
} // namespace axgl

namespace axgl::gui
{

class Element;

class Page
{
public:
  struct Context : axgl::Service::Context
  {
    axgl::GuiService& gui_service;
    axgl::gui::Page& page;
    const glm::mat4& projection;
    axgl::gui::Element* parent;
  };
  virtual ~Page() = default;
  virtual void set_size(uint32_t width, uint32_t height) = 0;
  virtual void init(const axgl::Service::Context& context) = 0;
  virtual void render(const axgl::Service::Context& context) = 0;
  [[nodiscard]] virtual bool should_render() const = 0;
  [[nodiscard]] virtual glm::ivec2 get_size() const = 0;
  [[nodiscard]] virtual axgl::ptr_t<axgl::Texture> get_texture() = 0;
  [[nodiscard]] virtual axgl::Container<axgl::gui::Element>& elements() = 0;
};

} // namespace axgl::gui
