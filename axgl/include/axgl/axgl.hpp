#pragma once

#include <chrono>
#include <cstdint>
#include <format>
#include <memory>
#include <string>
#include <unordered_map>

#include <axgl/common.hpp>
#include <axgl/interface/context.hpp>
#include <axgl/interface/services/camera_service.hpp>
#include <axgl/interface/services/entity_service.hpp>
#include <axgl/interface/services/gui_service.hpp>
#include <axgl/interface/services/input_service.hpp>
#include <axgl/interface/services/model_service.hpp>
#include <axgl/interface/services/realm_service.hpp>
#include <axgl/interface/services/renderer_service.hpp>
#include <axgl/interface/services/resource_service.hpp>
#include <axgl/interface/services/text_service.hpp>
#include <axgl/interface/services/window_service.hpp>

#include "service_container.hpp"

namespace axgl
{

#define __AXGL_DECLARE_SERVICE_GETTER(service_type, service_getter_name)                                               \
  ptr_t<service_type> service_getter_name##_service() const                                                            \
  {                                                                                                                    \
    return get_service<service_type>(service_type::kTypeId.data());                                                    \
  }

class Axgl final : public ServiceContainer
{
  axgl::Context context_{this};

public:
  void initialize() const override
  {
#if SPDLOG_ACTIVE_LEVEL == SPDLOG_LEVEL_DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif
#ifdef AXGL_DEBUG
    CPPTRACE_TRY
    {
#endif
      ServiceContainer::set_context(&context_);
      ServiceContainer::initialize();
#ifdef AXGL_DEBUG
    }
    CPPTRACE_CATCH(const std::exception& e)
    {
      // AXGL_LOG_ERROR(
      //   "Exception thrown duration initialization: {}\n{}", e.what(),
      //   cpptrace::from_current_exception().to_string(true));
      AXGL_LOG_DEBUG("Hello world");
    }
#endif
  }

  void terminate() const override
  {
#ifdef AXGL_DEBUG
    CPPTRACE_TRY
    {
#endif
      ServiceContainer::terminate();
#ifdef AXGL_DEBUG
    }
    CPPTRACE_CATCH(const std::exception& e)
    {
      AXGL_LOG_ERROR(
        "Exception thrown duration termination: {}\n{}", e.what(), cpptrace::from_current_exception().to_string(true));
    }
#endif
  }

  void run()
  {
#ifdef AXGL_DEBUG
    CPPTRACE_TRY
    {
#endif
      using namespace std::chrono;
      constexpr std::int64_t kOneSecond = 1000000000;
      constexpr double kTimeStep = kOneSecond / 60.0;

      auto start_time = high_resolution_clock::now();
      double delta_tick = 0.0;
#ifdef AXGL_DEBUG
      double debug_delta_time = 0.0;
      std::int64_t update_count = 0;
#endif

      on_start();
      while (running())
      {
        AXGL_PROFILE_SCOPE("Main Loop");

        const auto now = high_resolution_clock::now();
        const auto delta_time = static_cast<double>(duration_cast<nanoseconds>(now - start_time).count());
        delta_tick += delta_time / kTimeStep;
        start_time = now;
#ifdef AXGL_DEBUG
        debug_delta_time += delta_time;
        if (debug_delta_time >= kOneSecond)
        {
          AXGL_PLOT("Update hz", update_count);
          update_count = 0;
          debug_delta_time = 0;
        }
#endif

        const auto should_update = delta_tick >= 1;
        if (should_update)
        {
          AXGL_PLOT("Delta time (ns)", delta_time);
          AXGL_PLOT("Delta tick", std::round(delta_tick * 100.0) / 100.0);
          AXGL_PROFILE_SCOPE("Update");
          context_.delta_tick = delta_tick;
          context_.delta_tick_f = static_cast<float>(delta_tick);
          update();
#ifdef AXGL_DEBUG
          ++update_count;
#endif
        }

        while (delta_tick >= 1)
        {
          AXGL_PROFILE_SCOPE("Tick");
          tick();
          delta_tick--;
        }

        if (should_update)
        {
          AXGL_PROFILE_SCOPE("Render");
          render();
        }
      }
      on_end();
#ifdef AXGL_DEBUG
    }
    CPPTRACE_CATCH(const std::exception& e)
    {
      AXGL_LOG_ERROR("Exception thrown: {}\n{}", e.what(), cpptrace::from_current_exception().to_string(true));
    }
#endif
  }

  __AXGL_DECLARE_SERVICE_GETTER(axgl::WindowService, window)
  __AXGL_DECLARE_SERVICE_GETTER(axgl::RendererService, renderer)
  __AXGL_DECLARE_SERVICE_GETTER(axgl::ResourceService, resource)
  __AXGL_DECLARE_SERVICE_GETTER(axgl::RealmService, realm)
  __AXGL_DECLARE_SERVICE_GETTER(axgl::EntityService, entity)
  __AXGL_DECLARE_SERVICE_GETTER(axgl::InputService, input)
  __AXGL_DECLARE_SERVICE_GETTER(axgl::ModelService, model)
  __AXGL_DECLARE_SERVICE_GETTER(axgl::CameraService, camera)
  __AXGL_DECLARE_SERVICE_GETTER(axgl::TextService, text)
  __AXGL_DECLARE_SERVICE_GETTER(axgl::GuiService, gui)
};

} // namespace axgl

#include <axgl/impl/components/camera.hpp>
#include <axgl/impl/components/light.hpp>
#include <axgl/impl/services/camera_service.hpp>
#include <axgl/impl/services/entity_service.hpp>
#include <axgl/impl/services/realm_service.hpp>
#include <axgl/impl/services/resource_service.hpp>

namespace axgl
{

inline void configure_default(Axgl& axgl)
{
  axgl.register_service_t<impl::CameraService>();
  axgl.register_service_t<impl::EntityService>();
  axgl.register_service_t<impl::RealmService>();
  axgl.register_service_t<impl::ResourceService>();

  axgl.entity_service()->register_component_t<impl::component::Camera>();
  axgl.entity_service()->register_component_t<impl::component::Light>();
}

} // namespace axgl
