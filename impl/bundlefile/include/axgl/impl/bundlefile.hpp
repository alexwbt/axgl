#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/services/bundlefile_service.hpp>

namespace axgl
{
AXGL_DECLARE_REGISTER_SERVICE(impl::bundlefile::BundlefileService)

inline void configure_bundlefile(Axgl& axgl) { axgl.register_service_t<impl::bundlefile::BundlefileService>(); }

} // namespace axgl
