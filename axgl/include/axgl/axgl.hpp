#pragma once

#include <chrono>
#include <string>
#include <format>
#include <memory>
#include <unordered_map>

#include <spdlog/spdlog.h>

#include <axgl/common.hpp>
#include <axgl/interface/service.hpp>
#include <axgl/interface/window.hpp>
#include <axgl/interface/renderer.hpp>
#include <axgl/interface/resource.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/interface/input.hpp>
#include <axgl/interface/model.hpp>
#include <axgl/interface/gui.hpp>
#include <axgl/impl/service_manager.hpp>
#include <axgl/util/string.hpp>

NAMESPACE_AXGL

namespace DefaultServices
{
  constexpr const char* kWindow = "window";
  constexpr const char* kRenderer = "renderer";
  constexpr const char* kResource = "resource";
  constexpr const char* kRealm = "realm";
  constexpr const char* kInput = "input";
  constexpr const char* kModel = "model";
  constexpr const char* kGui = "gui";
};

#define DECLARE_SERVICE_GETTER(service_name, service_getter_name) \
  std::shared_ptr<interface::service_name##Service> service_getter_name##_service() const { \
    return get_service<interface::service_name##Service>(DefaultServices::k##service_name); \
  }

class Axgl final : public ServiceManager
{
public:
  void run()
  {
#ifdef AXGL_DEBUG
    CPPTRACE_TRY
    {
#endif
    initialize(this);

    constexpr int64_t kOneSecond = 1000000000;
    constexpr double kTimeStep = kOneSecond / 60.0;

    auto start_time = std::chrono::high_resolution_clock::now();
    double delta_time = 0.0;

    while (running(this))
    {
      ZoneScopedN("Main Loop");

      auto now = std::chrono::high_resolution_clock::now();
      delta_time += (double)std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_time).count() / kTimeStep;
      start_time = now;

      auto should_update = delta_time >= 1;

      while (delta_time >= 1)
      {
        ZoneScopedN("Update");
        update(this);
        delta_time--;
      }

      if (should_update)
      {
        ZoneScopedN("Render");
        render(this);
      }
    }

    terminate(this);

#ifdef AXGL_DEBUG
    } CPPTRACE_CATCH(const std::exception& e)
    {
      SPDLOG_ERROR("Exception thrown: {}\n{}", e.what(),
        cpptrace::from_current_exception().to_string(true));
    }
#endif
  }

  template<typename ServiceType>
  std::shared_ptr<ServiceType> use_service()
  {
#ifdef AXGL_DEBUG
    throw std::runtime_error(
      std::format("Service type '{}' is not supported.",
        typeid(ServiceType).name()));
#else
    return nullptr;
#endif
  }

  DECLARE_SERVICE_GETTER(Window, window)
  DECLARE_SERVICE_GETTER(Renderer, renderer)
  DECLARE_SERVICE_GETTER(Resource, resource)
  DECLARE_SERVICE_GETTER(Realm, realm)
  DECLARE_SERVICE_GETTER(Input, input)
  DECLARE_SERVICE_GETTER(Model, model)
  DECLARE_SERVICE_GETTER(Gui, gui)
};

NAMESPACE_AXGL_END
