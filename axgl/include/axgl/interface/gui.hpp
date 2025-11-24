#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/service.hpp>
#include <axgl/interface/entity/gui_element.hpp>

namespace axgl
{
namespace interface
{

class GuiService : virtual public Service
{
public:
  virtual std::shared_ptr<entity::GuiElement> create_gui_element() = 0;
};

} // namespace interface
} // namespace axgl
