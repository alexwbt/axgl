
add_library(net INTERFACE)

target_link_libraries(net
  INTERFACE
  util
)

target_include_directories(net
  INTERFACE
  ${CMAKE_SOURCE_DIR}/_external/asio/asio/include
  ./include
)

include(${CMAKE_SOURCE_DIR}/_cmake/compile_proxy.cmake)
add_compile_proxy(net "${CMAKE_CURRENT_SOURCE_DIR}/include")
