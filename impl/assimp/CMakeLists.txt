
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

include(${CMAKE_SOURCE_DIR}/_cmake/header_proxy.cmake)
add_header_proxy(axgl_assimp_impl ./axgl_assimp_impl.cpp)
