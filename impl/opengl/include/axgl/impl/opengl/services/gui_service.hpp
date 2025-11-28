#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/opengl/components/gui.hpp>
#include <axgl/impl/service_base.hpp>

namespace axgl::impl::opengl
{

class GuiService : public impl::ServiceBase
{
public:
  void initialize() override
  {
    get_context()->axgl->entity_service()->register_component_t<axgl::impl::opengl::component::Gui>();
  }
};

} // namespace axgl::impl::opengl
