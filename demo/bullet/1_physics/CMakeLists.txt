
add_executable(demo_bullet_physics)

target_sources(demo_bullet_physics
  PRIVATE
  "./main.cpp"
)

target_link_libraries(demo_bullet_physics
  PRIVATE
  axgl_bullet_impl
)
