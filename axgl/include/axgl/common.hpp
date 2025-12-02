#pragma once

#ifdef AXGL_DEBUG
#include <cpptrace/cpptrace.hpp>
#include <cpptrace/from_current.hpp>
#include <stdexcept>
#endif

#include <cstdint>
#include <string>
#include <string_view>

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <tracy/Tracy.hpp>

namespace axgl
{

template <typename T>
using ptr_t = std::shared_ptr<T>;

template <typename T, typename... Args>
ptr_t<T> create_ptr(Args&&... args) noexcept
{
  return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T1, typename T2>
ptr_t<T1> ptr_cast(ptr_t<T2> ptr) noexcept
{
  return std::dynamic_pointer_cast<T1>(ptr);
}

} // namespace axgl

#ifdef AXGL_DEBUG
#define AXGL_PROFILE_SCOPE(scope_name) ZoneScopedN(scope_name)
#else
#define AXGL_PROFILE_SCOPE(scope_name)
#endif
