include(${CMAKE_SOURCE_DIR}/_cmake/resource.cmake)

add_library(axgl_opengl_impl)

target_include_directories(axgl_opengl_impl
  PUBLIC
  ${CMAKE_SOURCE_DIR}/_external/stb
  ./include
)

target_link_libraries(axgl_opengl_impl
  PUBLIC
  glad
  utf8cpp
  freetype
  axgl_glfw_impl
)

embed_resource(axgl_opengl_impl res)
