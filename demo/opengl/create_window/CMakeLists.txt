
add_executable(demo_create_window)

target_sources(demo_create_window
  PRIVATE
  "./main.cpp"
)

target_link_libraries(demo_create_window
  PRIVATE
  axgl
  axgl_glfw_impl
)
