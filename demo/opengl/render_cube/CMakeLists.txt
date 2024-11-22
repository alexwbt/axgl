
add_executable(demo_render_cube)

target_sources(demo_render_cube
  PRIVATE
  "./main.cpp"
)

target_link_libraries(demo_render_cube
  PRIVATE
  axgl_opengl_impl
)
