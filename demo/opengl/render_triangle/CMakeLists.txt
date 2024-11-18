
add_executable(demo_render_triangle)

target_sources(demo_render_triangle
  PRIVATE
  "./main.cpp"
)

target_link_libraries(demo_render_triangle
  PRIVATE
  axgl
  axgl_opengl_impl
)
