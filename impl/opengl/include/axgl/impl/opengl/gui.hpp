#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/gui.hpp>
#include <axgl/interface/entity/gui_element.hpp>

#include <axgl/impl/service.hpp>
#include <axgl/impl/opengl/entity/gui_element.hpp>

NAMESPACE_AXGL_IMPL

class OpenglGuiService : public ServiceBase, public interface::GuiService
{
public:
  std::shared_ptr<interface::entity::GuiElement> create_gui_element() override
  {
    return std::make_shared<entity::OpenglGuiElement>();
  }
};

NAMESPACE_AXGL_IMPL_END
