
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
