
add_executable(demo_opengl_triangle)

target_sources(demo_opengl_triangle
  PRIVATE
  "./main.cpp"
)

target_link_libraries(demo_opengl_triangle
  PRIVATE
  axgl_opengl_impl
)
