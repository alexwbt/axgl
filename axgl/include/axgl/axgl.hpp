#pragma once

#include "axgl/include.hpp"
#include "axgl/interface/window.hpp"
#include "axgl/interface/service.hpp"
#include "axgl/core/service_manager.hpp"

NAMESPACE_AXGL

class Axgl final
{
private:
  ServiceManager service_manager_;

public:
  void run()
  {
    service_manager_.initialize();

    constexpr int64_t kOneSecond = 1000000000;
    constexpr double kTimeStep = kOneSecond / 60.0;

    auto start_time = std::chrono::high_resolution_clock::now();
    double delta_time = 0.0;

    while (service_manager_.running())
    {
      auto now = std::chrono::high_resolution_clock::now();
      delta_time += (double)std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_time).count() / kTimeStep;
      start_time = now;

      auto should_update = delta_time >= 1;

      while (delta_time >= 1)
      {
        service_manager_.update();
        delta_time--;
      }

      if (should_update)
        service_manager_.render();
    }

    service_manager_.terminate();
  }

  auto create_window()
  {

  }
};

NAMESPACE_AXGL_END
