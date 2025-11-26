#pragma once

#include <axgl/interface/camera_mode.hpp>
#include <axgl/interface/entity.hpp>
#include <axgl/interface/service.hpp>

namespace axgl
{

class CameraService : virtual public Service
{
public:
  virtual void set_camera_mode(std::shared_ptr<axgl::CameraMode> camera_mode) = 0;
  virtual void set_camera(std::shared_ptr<axgl::Entity> camera_entity) = 0;

  virtual axgl::Camera* get_camera() = 0;
};

} // namespace axgl
