
add_library(axgl_entt_impl INTERFACE)

target_include_directories(axgl_entt_impl
  INTERFACE
  ./include
)

target_link_libraries(axgl_entt_impl
  INTERFACE
  axgl
  EnTT
  spdlog
)
