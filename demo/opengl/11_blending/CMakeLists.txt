
add_executable(demo_opengl_blending)

target_sources(demo_opengl_blending
  PRIVATE
  "./main.cpp"
)

target_link_libraries(demo_opengl_blending
  PRIVATE
  axgl_opengl_impl
)
