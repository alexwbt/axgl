#pragma once

#include <chrono>
#include <spdlog/spdlog.h>

#include "axgl/component.h"


NAMESPACE_AXGL

class Gameloop : public ComponentParent
{
public:
  void run()
  {
    try
    {
      initialize();

      constexpr int64_t kOneSecond = 1000000000;
      constexpr double kTimeStep = kOneSecond / 60.0;

      auto start_time = std::chrono::high_resolution_clock::now();
      double delta_time = 0.0;

      while (alive())
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
    catch (const std::exception& e)
    {
      SPDLOG_CRITICAL(e.what());
    }

  }
};

NAMESPACE_AXGL_END
