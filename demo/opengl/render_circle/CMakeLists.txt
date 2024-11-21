
add_executable(demo_render_circle)

target_sources(demo_render_circle
  PRIVATE
  "./main.cpp"
)

target_link_libraries(demo_render_circle
  PRIVATE
  axgl_opengl_impl
)
