#pragma once

#include <axgl/axgl.hpp>

#include <axgl/impl/components/camera.hpp>
#include <axgl/impl/components/light.hpp>

#include <axgl/impl/services/camera_service.hpp>
#include <axgl/impl/services/entity_service.hpp>
#include <axgl/impl/services/light_service.hpp>
#include <axgl/impl/services/realm_service.hpp>
#include <axgl/impl/services/resource_service.hpp>
#include <axgl/impl/services/sorted_render_service.hpp>

#define AXGL_DECLARE_REGISTER_SERVICE(service_type)                                                                    \
  template <>                                                                                                          \
  inline std::shared_ptr<service_type> Axgl::register_service_t()                                                      \
  {                                                                                                                    \
    const auto service = std::make_shared<service_type>();                                                             \
    register_service(service_type::kTypeId.data(), service);                                                           \
    return service;                                                                                                    \
  }

namespace axgl
{
AXGL_DECLARE_REGISTER_SERVICE(impl::ResourceService)
AXGL_DECLARE_REGISTER_SERVICE(impl::RealmService)
AXGL_DECLARE_REGISTER_SERVICE(impl::EntityService)
AXGL_DECLARE_REGISTER_SERVICE(impl::CameraService)
AXGL_DECLARE_REGISTER_SERVICE(impl::LightService)
AXGL_DECLARE_REGISTER_SERVICE(impl::SortedRenderService)

template <const char*>
void auto_config(Axgl& axgl)
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
