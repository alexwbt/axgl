#pragma once

#include <cstddef>

namespace axgl::impl::opengl::renderer
{

inline constexpr std::size_t kSunLightLimit = 8;
inline constexpr std::size_t kSunShadowLimit = 1;
inline constexpr std::size_t kSunShadowCascadeCount = 3;

inline constexpr std::size_t kSpotLightLimit = 32;
inline constexpr std::size_t kSpotShadowLimit = 8;

inline constexpr std::size_t kPointLightLimit = 32;
inline constexpr std::size_t kPointShadowLimit = 8;

} // namespace axgl::impl::opengl::renderer

