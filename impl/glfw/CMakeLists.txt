
add_library(axgl_glfw_impl INTERFACE)

target_include_directories(axgl_glfw_impl
  INTERFACE
  ./include
)

target_link_libraries(axgl_glfw_impl
  INTERFACE
  glfw
  spdlog
)
