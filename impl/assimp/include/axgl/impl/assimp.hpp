#pragma once

#include <axgl/configure.hpp>
#include <axgl/impl/assimp/model_service.hpp>

namespace axgl
{
AXGL_DECLARE_REGISTER_SERVICE(impl::assimp::ModelService)

inline void auto_config(Axgl& axgl)
{
  axgl.register_service_t<impl::RealmService>();
  axgl.register_service_t<impl::EntityService>();
  axgl.register_service_t<impl::CameraService>();
  axgl.register_service_t<impl::LightService>();
  axgl.register_service_t<impl::SortedRenderService>();

  axgl.entity_service()->register_component_t<impl::component::Camera>();
  axgl.entity_service()->register_component_t<impl::component::Light>();
}

} // namespace axgl
