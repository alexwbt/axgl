
add_executable(demo_opengl_input)

target_sources(demo_opengl_input
  PRIVATE
  "./main.cpp"
)

target_link_libraries(demo_opengl_input
  PRIVATE
  axgl_opengl_impl
)
