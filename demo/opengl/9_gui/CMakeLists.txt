include(${CMAKE_SOURCE_DIR}/_cmake/resource.cmake)

add_executable(demo_opengl_gui)

target_sources(demo_opengl_gui
  PRIVATE
  "./main.cpp"
)

target_link_libraries(demo_opengl_gui
  PRIVATE
  axgl_opengl_impl
  axgl_assimp_impl
  axgl_bundlefile_impl
)

embed_resource(demo_opengl_gui res)
