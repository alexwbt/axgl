
add_library(net)

file(GLOB_RECURSE source_cpp_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/*.cpp")

target_sources(net
  PRIVATE
  ${source_cpp_files}
)

target_include_directories(net
  PUBLIC
  ../_external/asio/asio/include
  ./include
)
