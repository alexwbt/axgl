#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>

NAMESPACE_AXGL_IMPL

namespace component
{

class Light : public ComponentBase
{
public:
  interface::Light light;

  void update() override
  {
    ZoneScopedN("Light Component Update");

    const auto transform = get_parent()->transform();
    light.position = transform->position;
    light.direction = transform->rotation;

    const auto context = get_context();
    context->lights.push_back(&light);
  }
};

}

NAMESPACE_AXGL_IMPL_END

NAMESPACE_AXGL

template<>
inline std::shared_ptr<impl::component::Light> interface::RealmService::create_component()
{
  return std::make_shared<impl::component::Light>();
}

NAMESPACE_AXGL_END
