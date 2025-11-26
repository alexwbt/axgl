
add_executable(demo_opengl_cube)

target_sources(demo_opengl_cube
  PRIVATE
  "./main.cpp"
)

target_link_libraries(demo_opengl_cube
  PRIVATE
  axgl_opengl_impl
)
