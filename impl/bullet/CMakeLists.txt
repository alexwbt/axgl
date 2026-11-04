include(${CMAKE_SOURCE_DIR}/_cmake/compile_proxy.cmake)

add_library(axgl_bullet_impl INTERFACE)

target_include_directories(axgl_bullet_impl
  INTERFACE
  ./include
  ${CMAKE_SOURCE_DIR}/_external/bullet/src
)

target_link_libraries(axgl_bullet_impl
  INTERFACE
  axgl
  BulletDynamics
  BulletCollision
  LinearMath
)

add_compile_proxy(axgl_bullet_impl "${CMAKE_CURRENT_SOURCE_DIR}/include")
