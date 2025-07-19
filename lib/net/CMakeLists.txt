
add_library(net INTERFACE)

target_include_directories(net
  INTERFACE
  ${CMAKE_SOURCE_DIR}/_external/asio/asio/include
  ./include
)
