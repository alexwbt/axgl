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
    axgl::gui::Element* parent;
  };
  virtual ~Page() = default;
  virtual void set_size(uint32_t width, uint32_t height) = 0;
  virtual void render(const axgl::Service::Context& context, axgl::ptr_t<axgl::Texture> texture) = 0;
  [[nodiscard]] virtual axgl::Container<axgl::gui::Element>& elements() = 0;
};

} // namespace axgl::gui
