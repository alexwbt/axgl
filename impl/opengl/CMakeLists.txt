include(${CMAKE_SOURCE_DIR}/_cmake/resource.cmake)

add_library(axgl_opengl_impl)

target_include_directories(axgl_opengl_impl
  PUBLIC
  ./include
  ${CMAKE_SOURCE_DIR}/_external/stb
  ${CMAKE_CURRENT_BINARY_DIR}/resources
)

target_link_libraries(axgl_opengl_impl
  PUBLIC
  glad
  assimp
  axgl_glfw_impl
)

use_resource(axgl_opengl_impl res)
