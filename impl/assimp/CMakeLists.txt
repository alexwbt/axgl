
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

include(${CMAKE_SOURCE_DIR}/_cmake/compile_proxy.cmake)
add_compile_proxy(axgl_assimp_impl "${CMAKE_CURRENT_SOURCE_DIR}/include")
