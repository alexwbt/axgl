include(${CMAKE_SOURCE_DIR}/_cmake/resource.cmake)

add_executable(demo_opengl_text)

target_sources(demo_opengl_text
  PRIVATE
  "./main.cpp"
)

target_link_libraries(demo_opengl_text
  PRIVATE
  axgl_opengl_impl
)

embed_resource(demo_opengl_text res)
