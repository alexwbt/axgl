#pragma once

#include <memory>

#include <axgl/common.hpp>
#include <axgl/interface/component/gui_element.hpp>

NAMESPACE_AXGL_INTERFACE

class GuiService : public Service
{
public:
  virtual std::shared_ptr<GuiElement> create_page() = 0;
  virtual std::shared_ptr<GuiElement> create_element() const = 0;
};

NAMESPACE_AXGL_INTERFACE_END
