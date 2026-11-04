add_executable(demo_physics)
target_sources(demo_physics PRIVATE "main.cpp")

if(AXGL_DEMO_USE_OPENGL_IMPL)
  target_link_libraries(demo_physics PRIVATE axgl_opengl_impl)
endif()

target_link_libraries(demo_physics PRIVATE axgl_bullet_impl)
