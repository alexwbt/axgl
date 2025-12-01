#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/assimp/model_service.hpp>

namespace axgl
{
AXGL_DECLARE_REGISTER_SERVICE(impl::assimp::ModelService)

inline void configure_assimp(Axgl& axgl) { axgl.register_service_t<impl::assimp::ModelService>(); }

} // namespace axgl
