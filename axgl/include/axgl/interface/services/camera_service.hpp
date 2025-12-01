#pragma once

#include <string_view>

#include <axgl/common.hpp>
#include <axgl/interface/camera.hpp>
#include <axgl/interface/camera_mode.hpp>
#include <axgl/interface/entity.hpp>
#include <axgl/interface/service.hpp>

namespace axgl
{

class CameraService : virtual public axgl::Service
{
public:
  static constexpr std::string_view kTypeId = "service::camera";

  virtual void set_camera_mode(axgl::ptr_t<axgl::CameraMode> camera_mode) = 0;
  virtual void set_camera(axgl::ptr_t<axgl::Entity> camera_entity) = 0;

  virtual axgl::Camera* get_camera() = 0;
};

} // namespace axgl
