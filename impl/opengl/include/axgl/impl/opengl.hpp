#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/opengl/components/mesh.hpp>
#include <axgl/impl/opengl/services/renderer_service.hpp>
#include <axgl/impl/opengl/services/text_service.hpp>

namespace axgl
{
AXGL_DECLARE_REGISTER_SERVICE(impl::opengl::RendererService)
AXGL_DECLARE_REGISTER_SERVICE(impl::opengl::TextService)

inline void configure_opengl(Axgl& axgl)
{
  axgl.register_service_t<impl::opengl::RendererService>();
  axgl.register_service_t<impl::opengl::TextService>();

  axgl.entity_service()->register_component_t<impl::opengl::component::Mesh>();
}

} // namespace axgl
