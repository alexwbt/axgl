
add_library(axgl_bullet_impl INTERFACE)

target_include_directories(axgl_bullet_impl
  INTERFACE
  ./include
)

target_link_libraries(axgl_bullet_impl
  INTERFACE
  axgl
  bullet
)
