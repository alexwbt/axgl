#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/container.hpp>

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
  struct Context
  {
    axgl::Axgl& axgl;
    axgl::GuiService& gui_service;
    axgl::gui::Page& page;
    axgl::gui::Element* parent;
  };
  virtual ~Page() = default;
  virtual void set_size(glm::ivec2 size) = 0;
  [[nodiscard]] virtual axgl::ptr_t<axgl::Texture> render() = 0;
  [[nodiscard]] virtual axgl::Container<axgl::gui::Element>& elements() = 0;
};

} // namespace axgl::gui
