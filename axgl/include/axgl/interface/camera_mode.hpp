#pragma once

#include <memory>

#include <axgl/interface/camera.hpp>
#include <axgl/interface/services/input_service.hpp>

namespace axgl
{

class CameraMode
{
public:
  virtual ~CameraMode() = default;
  virtual void bind_inputs(std::shared_ptr<InputService> input_service) = 0;
  virtual void unbind_inputs(std::shared_ptr<InputService> input_service) = 0;
  virtual void update(Camera& camera) = 0;
};

} // namespace axgl
