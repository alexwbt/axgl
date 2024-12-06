#pragma once

#include <span>
#include <stdint.h>

#include "axgl/namespace.hpp"
#include "axgl/interface/realm.hpp"

NAMESPACE_AXGL_INTERFACE

class ModelService : public Service
{
public:
  virtual std::shared_ptr<Entity> load_model(const std::span<const uint8_t> data) = 0;
};

NAMESPACE_AXGL_INTERFACE_END
