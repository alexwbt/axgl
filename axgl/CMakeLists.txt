
add_library(axgl INTERFACE)

target_include_directories(axgl
  INTERFACE
  ./include
)
target_link_libraries(axgl
  INTERFACE
  util
  glm
  spdlog
  cpptrace::cpptrace
  Tracy::TracyClient
)

include(${CMAKE_SOURCE_DIR}/_cmake/compile_proxy.cmake)
add_compile_proxy(axgl "${CMAKE_CURRENT_SOURCE_DIR}/include")
