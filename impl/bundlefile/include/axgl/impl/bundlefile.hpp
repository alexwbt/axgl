#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/services/bundlefile_service.hpp>

namespace axgl
{
inline void configure_bundlefile(Axgl& axgl) { axgl.register_service_t<impl::bundlefile::BundlefileService>(); }

} // namespace axgl
