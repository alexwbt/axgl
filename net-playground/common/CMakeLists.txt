
add_library(net_playground_common)

file(GLOB_RECURSE source_cpp_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/*.cpp")


target_include_directories(net_playground_common
  PUBLIC
  ./include
)

target_sources(net_playground_common
  PRIVATE
  ${source_cpp_files}
)

target_link_libraries(net_playground_common
  PUBLIC
  axgl
  net
)
