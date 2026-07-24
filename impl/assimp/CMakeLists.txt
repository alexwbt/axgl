
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

include(${CMAKE_SOURCE_DIR}/_cmake/debug_interface_source.cmake)
add_debug_interface_source(axgl_assimp_impl ./axgl_assimp_impl.cpp)
