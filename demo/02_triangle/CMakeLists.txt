add_executable(demo_triangle)
target_sources(demo_triangle PRIVATE "main.cpp")

if(AXGL_DEMO_USE_OPENGL_IMPL)
  target_link_libraries(demo_triangle PRIVATE axgl_opengl_impl)
endif()
