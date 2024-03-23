
add_executable(net_playground)

file(GLOB_RECURSE source_cpp_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/*.cpp")

target_include_directories(net_playground
  PRIVATE
  ./src
)

target_sources(net_playground
  PRIVATE
  ${source_cpp_files}
)

target_link_libraries(net_playground
  PRIVATE
  net_playground_common
)

include(${CMAKE_SOURCE_DIR}/_cmake/resource.cmake)
add_resource_dir(${CMAKE_CURRENT_SOURCE_DIR}/res)
