
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

include(${CMAKE_SOURCE_DIR}/_cmake/debug_interface_source.cmake)
add_debug_interface_source(axgl ./axgl.cpp)
