#pragma once

//
// cpp trace
//
#ifdef AXGL_DEBUG
  #include <cpptrace/cpptrace.hpp>
  #include <cpptrace/from_current.hpp>
  #include <stdexcept>
#endif

//
// common imports
//
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

//
// profiling
//
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
  if (!ptr) throw std::bad_alloc();
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
  if (!ptr) throw std::bad_alloc();
  AXGL_ALLOC(ptr, _Size);
  return ptr;
}
// ReSharper restore CppNonInlineFunctionDefinitionInHeaderFile
#endif

//
// logging
//
#define AXGL_LOG_LEVEL_DEBUG 0
#define AXGL_LOG_LEVEL_INFO 1
#define AXGL_LOG_LEVEL_WARN 2
#define AXGL_LOG_LEVEL_ERROR 3
#define AXGL_LOG_LEVEL_NONE 4

#ifndef AXGL_LOG_LEVEL
  #ifdef AXGL_DEBUG
    #define AXGL_LOG_LEVEL AXGL_LOG_LEVEL_DEBUG
  #else
    #define AXGL_LOG_LEVEL AXGL_LOG_LEVEL_WARN
  #endif
#endif

#if AXGL_LOG_LEVEL <= AXGL_LOG_LEVEL_DEBUG
  #define AXGL_LOG_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#else
  #define AXGL_LOG_DEBUG(...)
#endif
#if AXGL_LOG_LEVEL <= AXGL_LOG_LEVEL_INFO
  #define AXGL_LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#else
  #define AXGL_LOG_INFO(...)
#endif
#if AXGL_LOG_LEVEL <= AXGL_LOG_LEVEL_WARN
  #define AXGL_LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#else
  #define AXGL_LOG_WARN(...)
#endif
#if AXGL_LOG_LEVEL <= AXGL_LOG_LEVEL_ERROR
  #define AXGL_LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#else
  #define AXGL_LOG_ERROR(...)
#endif

//
// pointer type
//
namespace axgl
{

template <typename T>
using ptr_t = std::shared_ptr<T>;

template <typename T>
using ref_t = std::weak_ptr<T>;

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
