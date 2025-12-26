#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/services/gui_service.hpp>

#include <axgl/impl/opengl/gui/element.hpp>
#include <axgl/impl/opengl/gui/page.hpp>

namespace axgl::impl::opengl
{

class GuiService : virtual public axgl::GuiService
{
  axgl::ptr_t<axgl::gui::Page> main_ui_;

public:
  axgl::ptr_t<axgl::gui::Page> create_page() override { return axgl::create_ptr<axgl::impl::opengl::gui::Page>(); }

  axgl::ptr_t<axgl::gui::Element> create_element() override
  {
    return axgl::create_ptr<axgl::impl::opengl::gui::Element>();
  }

  void set_main_ui(axgl::ptr_t<axgl::gui::Page> main_ui) override { main_ui_ = std::move(main_ui); }
  [[nodiscard]] axgl::ptr_t<axgl::gui::Page> get_main_ui() const override { return main_ui_; }

  void update(const Context& context) override
  {
    if (!main_ui_)
      return;
  }
};

} // namespace axgl::impl::opengl
