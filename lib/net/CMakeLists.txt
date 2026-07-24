
add_library(net INTERFACE)

target_include_directories(net
  INTERFACE
  ${CMAKE_SOURCE_DIR}/_external/asio/asio/include
  ./include
)

include(${CMAKE_SOURCE_DIR}/_cmake/debug_interface_source.cmake)
add_debug_interface_source(net ./net.cpp)
