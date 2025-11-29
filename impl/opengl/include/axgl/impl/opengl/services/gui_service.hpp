#pragma once

#include <axgl/interface/service.hpp>

#include <axgl/impl/opengl/components/gui.hpp>

namespace axgl::impl::opengl
{

class GuiService : public axgl::Service
{
public:
  void initialize(const Service::Context& context) override
  {
    context.axgl.entity_service()->register_component_t<axgl::impl::opengl::component::Gui>();
  }
};

} // namespace axgl::impl::opengl
