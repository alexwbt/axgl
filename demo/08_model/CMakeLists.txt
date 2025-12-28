add_executable(demo_model)
target_sources(demo_model PRIVATE "main.cpp")

if(AXGL_DEMO_USE_OPENGL_IMPL)
  target_link_libraries(demo_model
    PRIVATE
    axgl_opengl_impl
    axgl_assimp_impl
    axgl_bundlefile_impl
  )
endif()

include(${CMAKE_SOURCE_DIR}/_cmake/resource.cmake)
bundle_resource(demo_model res)
