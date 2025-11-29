#pragma once

#include <chrono>
#include <format>
#include <memory>
#include <string>
#include <unordered_map>

#ifdef AXGL_DEBUG
#include <cpptrace/cpptrace.hpp>
#include <cpptrace/from_current.hpp>
#include <stdexcept>
#endif

#include <spdlog/spdlog.h>
#include <tracy/Tracy.hpp>

#include <axgl/interface/services/camera_service.hpp>
#include <axgl/interface/services/entity_service.hpp>
#include <axgl/interface/services/input_service.hpp>
#include <axgl/interface/services/light_service.hpp>
#include <axgl/interface/services/model_service.hpp>
#include <axgl/interface/services/realm_service.hpp>
#include <axgl/interface/services/renderer_service.hpp>
#include <axgl/interface/services/resource_service.hpp>
#include <axgl/interface/services/window_service.hpp>

#include "service_container.hpp"

namespace axgl
{

namespace DefaultServices
{

constexpr auto kWindow = "window";
constexpr auto kRenderer = "renderer";
constexpr auto kResource = "resource";
constexpr auto kRealm = "realm";
constexpr auto kEntity = "entity";
constexpr auto kInput = "input";
constexpr auto kModel = "model";
constexpr auto kCamera = "camera";
constexpr auto kLight = "light";

}; // namespace DefaultServices

#define AXGL_DECLARE_SERVICE_GETTER(service_name, service_getter_name)                                                 \
  std::shared_ptr<service_name##Service> service_getter_name##_service() const                                         \
  {                                                                                                                    \
    return get_service<service_name##Service>(DefaultServices::k##service_name);                                       \
  }

class Axgl final : public ServiceContainer
{
public:
  void initialize()
  {
#ifdef AXGL_DEBUG
    CPPTRACE_TRY
    {
#endif
      ServiceContainer::initialize({*this});
#ifdef AXGL_DEBUG
    }
    CPPTRACE_CATCH(const std::exception& e)
    {
      SPDLOG_ERROR(
        "Exception thrown duration initialization: {}\n{}", e.what(),
        cpptrace::from_current_exception().to_string(true));
    }
#endif
  }

  void terminate()
  {
#ifdef AXGL_DEBUG
    CPPTRACE_TRY
    {
#endif
      ServiceContainer::terminate({*this});
#ifdef AXGL_DEBUG
    }
    CPPTRACE_CATCH(const std::exception& e)
    {
      SPDLOG_ERROR(
        "Exception thrown duration termination: {}\n{}", e.what(), cpptrace::from_current_exception().to_string(true));
    }
#endif
  }

  void run()
  {
    const Service::Context context{*this};
#ifdef AXGL_DEBUG
    CPPTRACE_TRY
    {
#endif
      constexpr int64_t kOneSecond = 1000000000;
      constexpr double kTimeStep = kOneSecond / 60.0;

      auto start_time = std::chrono::high_resolution_clock::now();
      double delta_time = 0.0;

      on_start(context);
      while (running(context))
      {
        ZoneScopedN("Main Loop");

        const auto now = std::chrono::high_resolution_clock::now();
        delta_time += std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_time).count() / kTimeStep;
        start_time = now;

        const auto should_update = delta_time >= 1;

        if (should_update)
        {
          ZoneScopedN("Update");
          update(context);
        }

        while (delta_time >= 1)
        {
          ZoneScopedN("Tick");
          tick(context);
          delta_time--;
        }

        if (should_update)
        {
          ZoneScopedN("Render");
          render(context);
        }
      }
      on_end(context);
#ifdef AXGL_DEBUG
    }
    CPPTRACE_CATCH(const std::exception& e)
    {
      SPDLOG_ERROR("Exception thrown: {}\n{}", e.what(), cpptrace::from_current_exception().to_string(true));
    }
#endif
  }

  template <typename ServiceType>
  std::shared_ptr<ServiceType> use_service()
  {
#ifdef AXGL_DEBUG
    throw std::runtime_error(std::format("Service type '{}' is not supported.", typeid(ServiceType).name()));
#else
    return nullptr;
#endif
  }

  AXGL_DECLARE_SERVICE_GETTER(Window, window)
  AXGL_DECLARE_SERVICE_GETTER(Renderer, renderer)
  AXGL_DECLARE_SERVICE_GETTER(Resource, resource)
  AXGL_DECLARE_SERVICE_GETTER(Realm, realm)
  AXGL_DECLARE_SERVICE_GETTER(Entity, entity)
  AXGL_DECLARE_SERVICE_GETTER(Input, input)
  AXGL_DECLARE_SERVICE_GETTER(Model, model)
  AXGL_DECLARE_SERVICE_GETTER(Camera, camera)
  AXGL_DECLARE_SERVICE_GETTER(Light, light)
};

} // namespace axgl

#include <axgl/impl/services/camera_service.hpp>
#include <axgl/impl/services/entity_service.hpp>
#include <axgl/impl/services/light_service.hpp>
#include <axgl/impl/services/realm_service.hpp>
#include <axgl/impl/services/resource_service.hpp>

#define AXGL_DECLARE_USE_SERVICE(service_name)                                                                         \
  template <>                                                                                                          \
  inline std::shared_ptr<impl::service_name##Service> Axgl::use_service()                                              \
  {                                                                                                                    \
    const auto service = std::make_shared<impl::service_name##Service>();                                              \
    register_service(DefaultServices::k##service_name, service);                                                       \
    return service;                                                                                                    \
  }

namespace axgl
{
AXGL_DECLARE_USE_SERVICE(Resource)
AXGL_DECLARE_USE_SERVICE(Realm)
AXGL_DECLARE_USE_SERVICE(Entity)
AXGL_DECLARE_USE_SERVICE(Camera)
AXGL_DECLARE_USE_SERVICE(Light)
} // namespace axgl
