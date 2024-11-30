
add_executable(demo_opengl_circle)

target_sources(demo_opengl_circle
  PRIVATE
  "./main.cpp"
)

target_link_libraries(demo_opengl_circle
  PRIVATE
  axgl_opengl_impl
)
