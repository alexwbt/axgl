add_executable(demo_circle)
target_sources(demo_circle PRIVATE "main.cpp")

if(AXGL_DEMO_USE_OPENGL_IMPL)
  target_link_libraries(demo_circle PRIVATE axgl_opengl_impl)
endif()
