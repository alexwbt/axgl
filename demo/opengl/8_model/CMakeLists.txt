include(${CMAKE_SOURCE_DIR}/_cmake/resource.cmake)

add_executable(demo_opengl_model)

target_sources(demo_opengl_model
  PRIVATE
  "./main.cpp"
)

target_link_libraries(demo_opengl_model
  PRIVATE
  axgl_opengl_impl
  axgl_assimp_impl
)

use_resource(demo_opengl_model res)
