
add_library(util INTERFACE)

target_include_directories(util
  INTERFACE
  ./include
)

include(${CMAKE_SOURCE_DIR}/_cmake/compile_proxy.cmake)
add_compile_proxy(util "${CMAKE_CURRENT_SOURCE_DIR}/include")
