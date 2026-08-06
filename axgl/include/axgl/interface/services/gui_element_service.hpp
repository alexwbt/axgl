#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/gui/element.hpp>
#include <axgl/interface/service.hpp>

namespace axgl
{

class GuiElementService : virtual public axgl::Service
{
public:
  static constexpr std::string_view kTypeId = "service:gui-element";

  virtual void register_element_factory(
    const std::string& type_id,
    std::function<axgl::ptr_t<axgl::gui::Element>()> element_factory)
    = 0;

  virtual axgl::ptr_t<axgl::gui::Element> create_element() = 0;
  virtual axgl::ptr_t<axgl::gui::Element> create_element(
    const std::string& type_id)
    = 0;

  template <typename ElementType>
  void register_element_t()
  {
    register_entity_factory(
      ElementType::kTypeId.data(),
      [] { return axgl::create_ptr<ElementType>(); });
  }

  template <typename ElementType>
  axgl::ptr_t<ElementType> create_element_t()
  {
    const auto type = ElementType::kTypeId.data();
    auto element = axgl::ptr_cast<ElementType>(create_component(type));
#ifdef AXGL_DEBUG
    if (!element)
      throw std::runtime_error(
        std::format(
          "Failed to create element: {} ({})", type,
          typeid(ElementType).name()));
#endif
    return element;
  }
};

} // namespace axgl
