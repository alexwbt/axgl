#pragma once

#include <stdexcept>

#include "axgl/namespace.hpp"
#include "axgl/interface/window.hpp"
#include "axgl/interface/service.hpp"
#include "axgl/core/service_manager.hpp"

NAMESPACE_AXGL

class Axgl final : private ServiceManager
{
public:
  void run()
  {
    initialize();

    constexpr int64_t kOneSecond = 1000000000;
    constexpr double kTimeStep = kOneSecond / 60.0;

    auto start_time = std::chrono::high_resolution_clock::now();
    double delta_time = 0.0;

    while (running())
    {
      auto now = std::chrono::high_resolution_clock::now();
      delta_time += (double)std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_time).count() / kTimeStep;
      start_time = now;

      auto should_update = delta_time >= 1;

      while (delta_time >= 1)
      {
        update();
        delta_time--;
      }

      if (should_update)
        render();
    }

    terminate();
  }

  template<typename ServiceType>
  std::shared_ptr<ServiceType> use_service()
  {
    throw std::runtime_error(
      std::format("Service type '{}' is not supported.",
        typeid(ServiceType).name()));
  }
};

NAMESPACE_AXGL_END
