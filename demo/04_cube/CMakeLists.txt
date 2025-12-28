add_executable(demo_cube)
target_sources(demo_cube PRIVATE "main.cpp")

if(AXGL_DEMO_USE_OPENGL_IMPL)
  target_link_libraries(demo_cube PRIVATE axgl_opengl_impl)
endif()
