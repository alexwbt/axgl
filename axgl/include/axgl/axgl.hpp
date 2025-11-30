#pragma once

#include <chrono>
#include <cstdint>
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

#define AXGL_DECLARE_SERVICE_GETTER(service_type, service_getter_name)                                                 \
  std::shared_ptr<service_type> service_getter_name##_service() const                                                  \
  {                                                                                                                    \
    return get_service<service_type>(service_type::kTypeId.data());                                                    \
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
      using namespace std::chrono;
      constexpr std::int64_t kOneSecond = 1000000000;
      constexpr double kTimeStep = kOneSecond / 60.0;

      auto start_time = high_resolution_clock::now();
      double delta_time = 0.0;

      on_start(context);
      while (running(context))
      {
        ZoneScopedN("Main Loop");

        const auto now = high_resolution_clock::now();
        delta_time += static_cast<double>(duration_cast<nanoseconds>(now - start_time).count()) / kTimeStep;
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
      SPDLOG_CRITICAL("Exception thrown: {}\n{}", e.what(), cpptrace::from_current_exception().to_string(true));
    }
#endif
  }

  template <typename ServiceType>
  std::shared_ptr<ServiceType> register_service_t()
  {
    SPDLOG_CRITICAL("Service type '{}' is not supported.", typeid(ServiceType).name());
    return nullptr;
  }

  AXGL_DECLARE_SERVICE_GETTER(axgl::WindowService, window)
  AXGL_DECLARE_SERVICE_GETTER(axgl::RendererService, renderer)
  AXGL_DECLARE_SERVICE_GETTER(axgl::ResourceService, resource)
  AXGL_DECLARE_SERVICE_GETTER(axgl::RealmService, realm)
  AXGL_DECLARE_SERVICE_GETTER(axgl::EntityService, entity)
  AXGL_DECLARE_SERVICE_GETTER(axgl::InputService, input)
  AXGL_DECLARE_SERVICE_GETTER(axgl::ModelService, model)
  AXGL_DECLARE_SERVICE_GETTER(axgl::CameraService, camera)
  AXGL_DECLARE_SERVICE_GETTER(axgl::LightService, light)
};

} // namespace axgl
