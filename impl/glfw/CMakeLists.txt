
add_library(axgl_glfw_impl INTERFACE)

target_include_directories(axgl_glfw_impl
  INTERFACE
  ./include
)

target_link_libraries(axgl_glfw_impl
  INTERFACE
  axgl
  glfw
)

include(${CMAKE_SOURCE_DIR}/_cmake/compile_proxy.cmake)
add_compile_proxy(axgl_glfw_impl "${CMAKE_CURRENT_SOURCE_DIR}/include")
