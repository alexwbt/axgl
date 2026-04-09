#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/entity_base.hpp>

class BunnyEntity : public axgl::impl::EntityBase
{
public:
  static constexpr std::string_view kTypeId = "entity::bunny";

  void on_create() override
  {
    axgl_->model_service()->load_model(*this, "bunny.glb");
    transform().origin = {0.25f, 0.25f, 0.25f};
  }
};
