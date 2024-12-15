include(${CMAKE_SOURCE_DIR}/_cmake/resource.cmake)

add_executable(demo_opengl_texture)

target_sources(demo_opengl_texture
  PRIVATE
  "./main.cpp"
)

target_link_libraries(demo_opengl_texture
  PRIVATE
  axgl_opengl_impl
)

embed_resource(demo_opengl_texture res)
