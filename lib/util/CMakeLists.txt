
add_library(util INTERFACE)

target_include_directories(util
  INTERFACE
  ./include
)

include(${CMAKE_SOURCE_DIR}/_cmake/debug_interface_source.cmake)
add_debug_interface_source(util ./util.cpp)
