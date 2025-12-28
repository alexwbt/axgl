add_executable(demo_blending)
target_sources(demo_blending PRIVATE "main.cpp")

if(AXGL_DEMO_USE_OPENGL_IMPL)
  target_link_libraries(demo_blending PRIVATE axgl_opengl_impl)
endif()
