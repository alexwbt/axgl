add_executable(demo_instancing)
target_sources(demo_instancing PRIVATE "src/main.cpp")

if(AXGL_DEMO_USE_OPENGL_IMPL)
  target_link_libraries(demo_instancing PRIVATE axgl_opengl_impl)
endif()

include(${CMAKE_SOURCE_DIR}/_cmake/resource.cmake)
embed_resource(demo_instancing res)
