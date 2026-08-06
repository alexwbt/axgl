#pragma once

#include <functional>
#include <unordered_map>

#include <axgl/common.hpp>
#include <axgl/interface/services/gui_element_service.hpp>

#include <axgl/impl/opengl/gui/element.hpp>
#include <axgl/impl/service_base.hpp>

namespace axgl::impl::opengl
{

class GuiElementService : virtual public axgl::GuiElementService,
                          public axgl::impl::ServiceBase
{
  using FactoryFunction = std::function<axgl::ptr_t<axgl::gui::Element>()>;

  std::unordered_map<std::string, FactoryFunction> element_factories_;

public:
  void register_element_factory(
    const std::string& type, FactoryFunction element_factory) override
  {
    element_factories_.emplace(type, element_factory);
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
};

} // namespace axgl::impl::opengl
