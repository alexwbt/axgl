#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/context.hpp>

namespace axgl
{
class GuiService;
}

namespace axgl::gui
{

class Page;
class Element;

struct Context : axgl::Context
{
  axgl::GuiService& gui_service;
  axgl::gui::Page& page;
  axgl::gui::Element* parent;
  float scale = 1.0f;
  const glm::mat4& projection;
};

} // namespace axgl::gui
