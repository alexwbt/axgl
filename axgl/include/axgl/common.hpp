#pragma once

/* Namespaces */

#define NAMESPACE_AXGL                                                                                                 \
  namespace axgl                                                                                                       \
  {
#define NAMESPACE_AXGL_END }

#define NAMESPACE_AXGL_INTERFACE                                                                                       \
  NAMESPACE_AXGL namespace interface                                                                                   \
  {
#define NAMESPACE_AXGL_INTERFACE_END                                                                                   \
  }                                                                                                                    \
  NAMESPACE_AXGL_END

#define NAMESPACE_AXGL_IMPL                                                                                            \
  NAMESPACE_AXGL namespace impl                                                                                        \
  {
#define NAMESPACE_AXGL_IMPL_END                                                                                        \
  }                                                                                                                    \
  NAMESPACE_AXGL_END

/* Debug */

#ifdef AXGL_DEBUG

#include <stdexcept>
#include <cpptrace/cpptrace.hpp>
#include <cpptrace/from_current.hpp>

#endif

#include <tracy/Tracy.hpp>
