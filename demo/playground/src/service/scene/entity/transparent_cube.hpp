#pragma once

#include "cube.hpp"

class TransparentCubeEntity : public CubeEntity
{
public:
  static constexpr std::string_view kTypeId = "entity::transparent-cube";

protected:
  [[nodiscard]] axgl::ptr_t<axgl::Material> create_material() const override
  {
    const auto material = CubeEntity::create_material();
    material->set_color({1.0f, 0.0f, 0.0f, 0.5f});
    material->set_enable_blend(true);
    return material;
  }
};
