
add_library(axgl_opengl_impl)

target_include_directories(axgl_opengl_impl
  PUBLIC
  ./include
)

target_link_libraries(axgl_opengl_impl
  PUBLIC
  axgl_glfw_impl
  glad
  glm
)

file(GLOB_RECURSE res_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "res/*")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src")
add_custom_command(
  OUTPUT src/res.cpp
  COMMAND embedfile "${CMAKE_CURRENT_SOURCE_DIR}/res" "${CMAKE_CURRENT_SOURCE_DIR}/src/res.cpp"
  DEPENDS ${res_files}
)

target_sources(axgl_opengl_impl
  PRIVATE
  src/res.cpp
)
