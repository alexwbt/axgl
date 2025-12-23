#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/services/gui_service.hpp>

#include <axgl/impl/opengl/gui/element.hpp>
#include <axgl/impl/opengl/gui/page.hpp>

namespace axgl::impl::opengl
{

class GuiService : virtual public axgl::GuiService
{
public:
  axgl::ptr_t<axgl::gui::Page> create_page() override
  {
    return axgl::create_ptr<axgl::impl::opengl::gui::Page>();
  }

  axgl::ptr_t<axgl::gui::Element> create_element() override
  {
    return axgl::create_ptr<axgl::impl::opengl::gui::Element>();
  }
};

} // namespace axgl::impl::opengl
