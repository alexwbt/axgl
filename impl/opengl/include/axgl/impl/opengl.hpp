#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/opengl/components/gui.hpp>
#include <axgl/impl/opengl/components/mesh.hpp>
#include <axgl/impl/opengl/services/gui_service.hpp>
#include <axgl/impl/opengl/services/renderer_service.hpp>
#include <axgl/impl/opengl/services/text_service.hpp>

namespace axgl
{
AXGL_DECLARE_REGISTER_SERVICE(impl::opengl::RendererService)
AXGL_DECLARE_REGISTER_SERVICE(impl::opengl::TextService)
AXGL_DECLARE_REGISTER_SERVICE(impl::opengl::GuiService)

inline void configure_opengl(Axgl& axgl)
{
  axgl.register_service_t<impl::opengl::RendererService>();
  axgl.register_service_t<impl::opengl::TextService>();
  axgl.register_service_t<impl::opengl::GuiService>();

  axgl.entity_service()->register_component_t<impl::opengl::component::Mesh>();
  axgl.entity_service()->register_component_t<impl::opengl::component::Gui>();
}

} // namespace axgl
