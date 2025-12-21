#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/service.hpp>

#include <axgl/interface/gui/element.hpp>
#include <axgl/interface/gui/page.hpp>

namespace axgl
{

class GuiService : virtual public axgl::Service
{
public:
  static constexpr std::string_view kTypeId = "service::gui";

  virtual axgl::ptr_t<axgl::gui::Page> create_page() = 0;
  virtual axgl::ptr_t<axgl::gui::Element> create_element() = 0;
};

} // namespace axgl
