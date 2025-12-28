add_executable(demo_texture)
target_sources(demo_texture PRIVATE "main.cpp")

if(AXGL_DEMO_USE_OPENGL_IMPL)
  target_link_libraries(demo_texture PRIVATE axgl_opengl_impl)
endif()

include(${CMAKE_SOURCE_DIR}/_cmake/resource.cmake)
embed_resource(demo_texture res)
