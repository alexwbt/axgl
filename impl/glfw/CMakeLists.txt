
add_library(axgl_glfw_impl INTERFACE)

target_include_directories(axgl_glfw_impl
  INTERFACE
  ./include
)

target_link_libraries(axgl_glfw_impl
  INTERFACE
  axgl
  glfw
)

include(${CMAKE_SOURCE_DIR}/_cmake/debug_interface_source.cmake)
add_debug_interface_source(axgl_glfw_impl ./axgl_glfw_impl.cpp)
