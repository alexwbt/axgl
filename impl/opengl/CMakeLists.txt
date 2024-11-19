include(${CMAKE_SOURCE_DIR}/_cmake/resource.cmake)

add_library(axgl_opengl_impl INTERFACE)

target_include_directories(axgl_opengl_impl
  INTERFACE
  ./include
  ${CMAKE_CURRENT_BINARY_DIR}/resources
)

target_link_libraries(axgl_opengl_impl
  INTERFACE
  axgl_glfw_impl
  glad
  glm
)

use_resource(axgl_opengl_impl res)
