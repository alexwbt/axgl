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

#include <axgl/interface/services/input_service.hpp>
#include <axgl/interface/services/model_service.hpp>
#include <axgl/interface/services/realm_service.hpp>
#include <axgl/interface/services/renderer_service.hpp>
#include <axgl/interface/services/resource_service.hpp>
#include <axgl/interface/services/window_service.hpp>

#include "./service_container.hpp"

namespace axgl
{

namespace DefaultServices
{

constexpr auto kWindow = "window";
constexpr auto kRenderer = "renderer";
constexpr auto kResource = "resource";
constexpr auto kRealm = "realm";
constexpr auto kInput = "input";
constexpr auto kModel = "model";

}; // namespace DefaultServices

#define AXGL_DECLARE_SERVICE_GETTER(service_name, service_getter_name)                                                 \
  std::shared_ptr<service_name##Service> service_getter_name##_service() const                                         \
  {                                                                                                                    \
    return get_service<service_name##Service>(DefaultServices::k##service_name);                                       \
  }

class Axgl final : public ServiceContainer
{
public:
  Axgl() { context_.axgl = this; }

  void run() const
  {
#ifdef AXGL_DEBUG
    CPPTRACE_TRY
    {
#endif
      initialize();

      constexpr int64_t kOneSecond = 1000000000;
      constexpr double kTimeStep = kOneSecond / 60.0;

      auto start_time = std::chrono::high_resolution_clock::now();
      double delta_time = 0.0;

      while (running())
      {
        ZoneScopedN("Main Loop");

        const auto now = std::chrono::high_resolution_clock::now();
        delta_time += std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_time).count() / kTimeStep;
        start_time = now;

        const auto should_update = delta_time >= 1;

        if (should_update)
        {
          ZoneScopedN("Update");
          update();
        }

        while (delta_time >= 1)
        {
          ZoneScopedN("Tick");
          tick();
          delta_time--;
        }

        if (should_update)
        {
          ZoneScopedN("Render");
          render();
        }
      }

      terminate();

#ifdef AXGL_DEBUG
    }
    CPPTRACE_CATCH(const std::exception& e)
    {
      SPDLOG_ERROR("Exception thrown: {}\n{}", e.what(), cpptrace::from_current_exception().to_string(true));
    }
#endif
  }

  AXGL_DECLARE_SERVICE_GETTER(Window, window)
  AXGL_DECLARE_SERVICE_GETTER(Renderer, renderer)
  AXGL_DECLARE_SERVICE_GETTER(Resource, resource)
  AXGL_DECLARE_SERVICE_GETTER(Realm, realm)
  AXGL_DECLARE_SERVICE_GETTER(Input, input)
  AXGL_DECLARE_SERVICE_GETTER(Model, model)
};

} // namespace axgl
