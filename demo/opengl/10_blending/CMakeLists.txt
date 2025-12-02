include(${CMAKE_SOURCE_DIR}/_cmake/resource.cmake)

add_executable(demo_opengl_blending)

target_include_directories(demo_opengl_blending
  PRIVATE
  ./include
)

target_sources(demo_opengl_blending
  PRIVATE
  "./main.cpp"
)

target_link_libraries(demo_opengl_blending
  PRIVATE
  axgl_opengl_impl
)

embed_resource(demo_opengl_blending res)
