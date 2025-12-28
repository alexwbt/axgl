add_executable(demo_textured_cube)
target_sources(demo_textured_cube PRIVATE "main.cpp")

if(AXGL_DEMO_USE_OPENGL_IMPL)
  target_link_libraries(demo_textured_cube PRIVATE axgl_opengl_impl)
endif()

include(${CMAKE_SOURCE_DIR}/_cmake/resource.cmake)
embed_resource(demo_textured_cube res)
