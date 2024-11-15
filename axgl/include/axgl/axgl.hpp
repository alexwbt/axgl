#pragma once

#include "axgl/index.hpp"
#include "axgl/service.hpp"

NAMESPACE_AXGL

class Axgl final
{
private:
  ServiceManager service_manager_;

public:
  void initialize()
  {
    service_manager_.initialize();
  }

  void terminate()
  {
    service_manager_.terminate();
  }
};

NAMESPACE_AXGL_END
