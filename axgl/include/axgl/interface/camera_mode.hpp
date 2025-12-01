#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/camera.hpp>
#include <axgl/interface/services/input_service.hpp>

namespace axgl
{

class CameraMode
{
public:
  virtual ~CameraMode() = default;
  virtual void bind_inputs(axgl::ptr_t<axgl::InputService> input_service) = 0;
  virtual void unbind_inputs(axgl::ptr_t<axgl::InputService> input_service) = 0;
  virtual void update(axgl::Camera& camera) = 0;
};

} // namespace axgl
