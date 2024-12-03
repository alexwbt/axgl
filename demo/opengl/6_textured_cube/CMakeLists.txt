include(${CMAKE_SOURCE_DIR}/_cmake/resource.cmake)

add_executable(demo_opengl_textured_cube)

target_sources(demo_opengl_textured_cube
  PRIVATE
  "./main.cpp"
)

target_link_libraries(demo_opengl_textured_cube
  PRIVATE
  axgl_opengl_impl
)

use_resource(demo_opengl_textured_cube res)
