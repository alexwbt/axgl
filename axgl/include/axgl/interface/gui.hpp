#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/service.hpp>
#include <axgl/interface/entity/gui_element.hpp>

NAMESPACE_AXGL_INTERFACE

class GuiService : virtual public Service
{
public:
  virtual std::shared_ptr<entity::GuiElement> create_gui_element() = 0;
};

NAMESPACE_AXGL_INTERFACE_END
