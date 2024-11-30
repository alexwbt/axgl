
add_executable(demo_opengl_window)

target_sources(demo_opengl_window
  PRIVATE
  "./main.cpp"
)

target_link_libraries(demo_opengl_window
  PRIVATE
  axgl_glfw_impl
)
