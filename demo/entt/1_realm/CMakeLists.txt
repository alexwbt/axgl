
add_executable(demo_entt_realm)

target_sources(demo_entt_realm
  PRIVATE
  "./main.cpp"
)

target_link_libraries(demo_entt_realm
  PRIVATE
  axgl_opengl_impl
  axgl_entt_impl
)
