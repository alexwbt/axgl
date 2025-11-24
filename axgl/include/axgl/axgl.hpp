#pragma once

#include <chrono>
#include <string>
#include <format>
#include <memory>
#include <unordered_map>

#include <spdlog/spdlog.h>

#include <axgl/common.hpp>
#include <axgl/interface/window.hpp>
#include <axgl/interface/renderer.hpp>
#include <axgl/interface/resource.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/interface/input.hpp>
#include <axgl/interface/model.hpp>
#include <axgl/impl/service.hpp>

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
  std::shared_ptr<interface::service_name##Service> service_getter_name##_service() const                              \
  {                                                                                                                    \
    return get_service<interface::service_name##Service>(DefaultServices::k##service_name);                            \
  }

class Axgl final : public impl::ServiceManager
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

  template <typename ServiceType> std::shared_ptr<ServiceType> use_service()
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
  AXGL_DECLARE_SERVICE_GETTER(Input, input)
  AXGL_DECLARE_SERVICE_GETTER(Model, model)
};

} // namespace axgl
