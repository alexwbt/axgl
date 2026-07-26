
add_library(axgl_bundlefile_impl INTERFACE)

target_include_directories(axgl_bundlefile_impl
  INTERFACE
  ./include
)

target_link_libraries(axgl_bundlefile_impl
  INTERFACE
  axgl
  bundlefile_lib
)

include(${CMAKE_SOURCE_DIR}/_cmake/compile_proxy.cmake)
add_compile_proxy(axgl_bundlefile_impl "${CMAKE_CURRENT_SOURCE_DIR}/include")
