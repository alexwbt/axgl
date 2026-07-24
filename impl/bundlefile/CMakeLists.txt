
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

include(${CMAKE_SOURCE_DIR}/_cmake/debug_interface_source.cmake)
add_debug_interface_source(axgl_bundlefile_impl ./axgl_bundlefile_impl.cpp)
