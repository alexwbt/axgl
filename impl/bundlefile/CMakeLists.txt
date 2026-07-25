
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

include(${CMAKE_SOURCE_DIR}/_cmake/header_proxy.cmake)
add_header_proxy(axgl_bundlefile_impl ./axgl_bundlefile_impl.cpp)
