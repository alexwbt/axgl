#pragma once

#include <unordered_map>

#include <axgl/common.hpp>
#include <axgl/interface/services/gui_service.hpp>

#include <axgl/impl/opengl/gui/element.hpp>
#include <axgl/impl/opengl/gui/page.hpp>
#include <axgl/impl/service_base.hpp>

namespace axgl::impl::opengl
{

class GuiService : virtual public axgl::GuiService, public axgl::impl::ServiceBase
{
  std::unordered_map<std::string, axgl::ptr_t<axgl::gui::Style>> styles_;

  axgl::ptr_t<axgl::gui::Page> main_ui_;
  std::vector<axgl::ptr_t<axgl::gui::Page>> pages_;

public:
  axgl::ptr_t<axgl::gui::Page> create_page() override
  {
    auto page = axgl::create_ptr<axgl::impl::opengl::gui::Page>();
    page->set_context(context_);
    pages_.emplace_back(page);
    return page;
  }

  axgl::ptr_t<axgl::gui::Element> create_element() override
  {
    return axgl::create_ptr<axgl::impl::opengl::gui::Element>();
  }

  axgl::ptr_t<axgl::gui::Style> create_style(const std::string& name) override
  {
#ifdef AXGL_DEBUG
    if (styles_.contains(name)) AXGL_LOG_WARN("GuiService: style \"{0}\" already exists, overriding style.", name);
#endif
    auto style = axgl::create_ptr<axgl::gui::Style>();
    styles_[name] = style;
    return style;
  }

  axgl::ptr_t<axgl::gui::Style> get_style(const std::string& name) const override
  {
    const auto it = styles_.find(name);
    if (it == styles_.end())
    {
#ifdef AXGL_DEBUG
      AXGL_LOG_WARN("GuiService: required style \"{}\" does not exist.", name);
#endif
      return nullptr;
    }
    return it->second;
  }

  void set_main_ui(axgl::ptr_t<axgl::gui::Page> main_ui) override { main_ui_ = std::move(main_ui); }
  [[nodiscard]] axgl::ptr_t<axgl::gui::Page> get_main_ui() const override { return main_ui_; }

  void update() override
  {
    AXGL_PROFILE_SCOPE("GUI Update");
    for (const auto& page : pages_)
      page->update();

    // reset updated
    for (const auto& entry : styles_)
      entry.second->reset_updated();
  }
};

} // namespace axgl::impl::opengl
