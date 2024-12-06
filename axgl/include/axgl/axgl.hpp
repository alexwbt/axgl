#pragma once

#include <chrono>
#include <string>
#include <format>
#include <memory>
#include <stdexcept>
#include <unordered_map>

#include "axgl/namespace.hpp"
#include "axgl/core/service_manager.hpp"
#include "axgl/interface/service.hpp"
#include "axgl/interface/input.hpp"
#include "axgl/interface/window.hpp"
#include "axgl/interface/renderer.hpp"
#include "axgl/interface/realm.hpp"
#include "axgl/util/string.hpp"

#include <spdlog/spdlog.h>

NAMESPACE_AXGL

class Axgl final : public ServiceManager
{
public:
  void run()
  {
    try
    {
      initialize(this);

      constexpr int64_t kOneSecond = 1000000000;
      constexpr double kTimeStep = kOneSecond / 60.0;

      auto start_time = std::chrono::high_resolution_clock::now();
      double delta_time = 0.0;

      while (running(this))
      {
        auto now = std::chrono::high_resolution_clock::now();
        delta_time += (double)std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_time).count() / kTimeStep;
        start_time = now;

        auto should_update = delta_time >= 1;

        while (delta_time >= 1)
        {
          update(this);
          delta_time--;
        }

        if (should_update)
          render(this);
      }

      terminate(this);
    }
    catch (const std::exception& e)
    {
      SPDLOG_ERROR(e.what());
    }
  }

  template<typename ServiceType>
  std::shared_ptr<ServiceType> use_service()
  {
    throw std::runtime_error(
      std::format("Service type '{}' is not supported.",
        typeid(ServiceType).name()));
  }

  std::shared_ptr<interface::WindowService> window_service() const
  {
    return get_service<interface::WindowService>("window");
  }

  std::shared_ptr<interface::RendererService> renderer_service() const
  {
    return get_service<interface::RendererService>("renderer");
  }

  std::shared_ptr<interface::RealmService> realm_service() const
  {
    return get_service<interface::RealmService>("realm");
  }

  std::shared_ptr<interface::InputService> input_service() const
  {
    return get_service<interface::InputService>("input");
  }
};

NAMESPACE_AXGL_END
