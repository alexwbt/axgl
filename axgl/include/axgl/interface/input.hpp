#pragma once

#include <vector>
#include <stdint.h>

#include "axgl/include.hpp"

NAMESPACE_AXGL_INTERFACE

class Input
{
public:
  virtual std::vector<uint32_t> get_input() = 0;
};

NAMESPACE_AXGL_INTERFACE_END
