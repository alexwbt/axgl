add_executable(demo_input)
target_sources(demo_input PRIVATE "main.cpp")

if(AXGL_DEMO_USE_OPENGL_IMPL)
  target_link_libraries(demo_input PRIVATE axgl_opengl_impl)
endif()
