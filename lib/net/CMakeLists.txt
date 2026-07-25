
add_library(net INTERFACE)

target_include_directories(net
  INTERFACE
  ${CMAKE_SOURCE_DIR}/_external/asio/asio/include
  ./include
)

include(${CMAKE_SOURCE_DIR}/_cmake/header_proxy.cmake)
add_header_proxy(net ./net.cpp)
