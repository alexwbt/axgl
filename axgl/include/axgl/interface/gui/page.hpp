#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/container.hpp>
#include <axgl/interface/input.hpp>
#include <axgl/interface/pointer.hpp>
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
    const axgl::gui::Element* parent;
    const axgl::gui::Page::Context* parent_context;
  };
  struct RenderContext : Context
  {
    const glm::mat4& projection;
  };
  struct InputContext : Context
  {
    axgl::InputService& input_service;
    const axgl::Pointer& pointer;
    const axgl::Input& activate;
    const axgl::Input& switch_focus;
  };
  virtual ~Page() = default;
  virtual void set_size(std::uint32_t width, std::uint32_t height) = 0;
  virtual void set_should_render(bool should_render) = 0;
  virtual void init(const axgl::Service::Context& context) = 0;
  virtual void update(const axgl::Service::Context& context) = 0;
  virtual void render(const axgl::Service::Context& context) = 0;
  [[nodiscard]] virtual bool should_render() const = 0;
  [[nodiscard]] virtual glm::ivec2 get_size() const = 0;
  [[nodiscard]] virtual axgl::ptr_t<axgl::Texture> get_texture() const = 0;
  [[nodiscard]] virtual axgl::Container<axgl::gui::Element>& elements() = 0;
};

} // namespace axgl::gui
