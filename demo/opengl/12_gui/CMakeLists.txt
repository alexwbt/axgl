
add_executable(demo_opengl_gui)

target_sources(demo_opengl_gui
  PRIVATE
  "./main.cpp"
)

target_link_libraries(demo_opengl_gui
  PRIVATE
  axgl_opengl_impl
)
