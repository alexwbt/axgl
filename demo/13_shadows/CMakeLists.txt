add_executable(demo_shadows)
target_sources(demo_shadows PRIVATE "src/main.cpp")

if(AXGL_DEMO_USE_OPENGL_IMPL)
  target_link_libraries(demo_shadows PRIVATE axgl_opengl_impl)
endif()
