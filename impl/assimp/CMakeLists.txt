
add_library(axgl_assimp_impl INTERFACE)

target_include_directories(axgl_assimp_impl
  INTERFACE
  ./include
)

target_link_libraries(axgl_assimp_impl
  INTERFACE
  axgl
  assimp
)
