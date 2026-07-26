
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

include(${CMAKE_SOURCE_DIR}/_cmake/header_proxy.cmake)
add_interface_proxy(axgl_glfw_impl ./axgl_glfw_impl.cpp)
