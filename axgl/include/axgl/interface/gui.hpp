#pragma once

#include <memory>

#include "axgl/namespace.hpp"
#include "axgl/interface/component/gui_element.hpp"

NAMESPACE_AXGL_INTERFACE

class GuiService : public Service
{
public:
  virtual void add_page(const std::string& id, std::shared_ptr<GuiElement> element) = 0;
  virtual void use_page(const std::string& id) = 0;
};

NAMESPACE_AXGL_INTERFACE_END
