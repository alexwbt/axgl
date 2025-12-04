#pragma once

#ifdef AXGL_DEBUG
#include <cpptrace/cpptrace.hpp>
#include <cpptrace/from_current.hpp>
#include <stdexcept>
#endif

#include <cstdint>
#include <span>
#include <string>
#include <string_view>

#ifdef AXGL_DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <tracy/Tracy.hpp>

#ifdef AXGL_DEBUG
#define AXGL_PROFILE_SCOPE(name) ZoneNamedN(TracyConcat(___tracy_scoped_zone, TracyLine), name, true)
#define AXGL_PLOT(name, value) TracyPlot(name, value)
#define AXGL_ALLOC(pointer, size) TracyAlloc(ptr, _Size);
#define AXGL_FREE(pointer) TracyFree(_Block);
#else
#define AXGL_PROFILE_SCOPE(name)
#define AXGL_PLOT(name, value)
#define AXGL_ALLOC(pointer, size)
#define AXGL_FREE(pointer)
#endif

#if defined(AXGL_DEBUG) && defined(TRACY_ENABLE)
// ReSharper disable CppNonInlineFunctionDefinitionInHeaderFile
void* operator new(const std::size_t _Size)
{
  const auto ptr = malloc(_Size);
  if (!ptr)
    throw std::bad_alloc();
  AXGL_ALLOC(ptr, _Size);
  return ptr;
}
void operator delete(void* _Block) noexcept
{
  if (_Block)
  {
    AXGL_FREE(_Block);
    free(_Block);
  }
}
void operator delete(void* _Block, std::size_t) noexcept { operator delete(_Block); }
void operator delete[](void* _Block) noexcept
{
  if (_Block)
  {
    AXGL_FREE(_Block);
    free(_Block);
  }
}
void* operator new[](const std::size_t _Size)
{
  const auto ptr = malloc(_Size);
  if (!ptr)
    throw std::bad_alloc();
  AXGL_ALLOC(ptr, _Size);
  return ptr;
}
// ReSharper restore CppNonInlineFunctionDefinitionInHeaderFile
#endif

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
