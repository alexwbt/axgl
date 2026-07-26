include(${CMAKE_SOURCE_DIR}/_cmake/resource.cmake)
include(${CMAKE_SOURCE_DIR}/_cmake/compile_proxy.cmake)

add_executable(demo_playground)
target_sources(demo_playground PRIVATE "src/main.cpp")

target_link_libraries(demo_playground PRIVATE axgl_assimp_impl)

if(AXGL_DEMO_USE_OPENGL_IMPL)
  target_link_libraries(demo_playground PRIVATE axgl_opengl_impl)
endif()

embed_resource(demo_playground res)

add_compile_proxy(demo_playground "${CMAKE_CURRENT_SOURCE_DIR}/src")
