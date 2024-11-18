
add_library(axgl_opengl_impl INTERFACE)

target_include_directories(axgl_opengl_impl
  INTERFACE
  ./include
)

target_link_libraries(axgl_opengl_impl
  INTERFACE
  axgl_glfw_impl
  glad
  glm
)
