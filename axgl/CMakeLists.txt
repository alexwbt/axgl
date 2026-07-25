
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

include(${CMAKE_SOURCE_DIR}/_cmake/header_proxy.cmake)
add_header_proxy(axgl ./axgl.cpp
  INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include"
  UMBRELLA_HEADER axgl/axgl.hpp
)
