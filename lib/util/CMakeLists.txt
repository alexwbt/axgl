
add_library(util INTERFACE)

target_include_directories(util
  INTERFACE
  ./include
)

include(${CMAKE_SOURCE_DIR}/_cmake/header_proxy.cmake)
add_header_proxy(util ./util.cpp)
