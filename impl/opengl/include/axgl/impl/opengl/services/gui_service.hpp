#pragma once

#include <functional>
#include <unordered_map>

#include <axgl/common.hpp>
#include <axgl/interface/services/gui_service.hpp>

#include <axgl/impl/opengl/gui/element.hpp>
#include <axgl/impl/opengl/gui/page.hpp>
#include <axgl/impl/service_base.hpp>

namespace axgl::impl::opengl
{

class GuiService : virtual public axgl::GuiService,
                   public axgl::impl::ServiceBase
{
  using FactoryFunction = std::function<axgl::ptr_t<axgl::gui::Element>()>;

  std::unordered_map<std::string, FactoryFunction> element_factories_;
  std::unordered_map<std::string, axgl::ptr_t<axgl::gui::Style>> styles_;

  bool initialized_ = false;
  axgl::ptr_t<axgl::gui::Page> main_ui_;

public:
  axgl::ptr_t<axgl::gui::Page> create_page() override
  {
    return axgl::create_ptr<axgl::impl::opengl::gui::Page>();
  }

  axgl::ptr_t<axgl::gui::Element> create_element() override
  {
    return axgl::create_ptr<axgl::impl::opengl::gui::Element>();
  }

  axgl::ptr_t<axgl::gui::Element> create_element(
    const std::string& type) override
  {
#ifdef AXGL_DEBUG
    if (!element_factories_.contains(type))
      throw std::runtime_error(
        std::format("Element factory for '{}' not registered.", type));
#endif
    return element_factories_.at(type)();
  }

  void register_element_factory(
    const std::string& type, FactoryFunction element_factory) override
  {
    element_factories_.emplace(type, element_factory);
  }

  axgl::ptr_t<axgl::gui::Style> create_style(const std::string& name) override
  {
#ifdef AXGL_DEBUG
    if (styles_.contains(name))
      AXGL_LOG_WARN(
        "GuiService: style \"{0}\" already exists, overriding style.", name);
#endif
    auto style = axgl::create_ptr<axgl::gui::Style>();
    styles_[name] = style;
    return style;
  }

  axgl::ptr_t<axgl::gui::Style> get_style(
    const std::string& name) const override
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

  void set_main_ui(axgl::ptr_t<axgl::gui::Page> main_ui) override
  {
    if (main_ui)
    {
      main_ui_ = std::move(main_ui);
      main_ui_->set_context(context_);
      initialized_ = false;
    }
    else main_ui_ = nullptr;
  }
  [[nodiscard]] axgl::ptr_t<axgl::gui::Page> get_main_ui() const override
  {
    return main_ui_;
  }

  void update() override
  {
    if (main_ui_)
    {
      if (!initialized_)
      {
        main_ui_->init();
        initialized_ = true;
      }
      AXGL_PROFILE_SCOPE("GUI Update");
      main_ui_->update();
    }

    // reset updated
    for (const auto& entry : styles_)
      entry.second->reset_modified();
  }
};

} // namespace axgl::impl::opengl

