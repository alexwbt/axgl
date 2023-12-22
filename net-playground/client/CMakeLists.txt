
add_executable(net_playrgound)

file(GLOB_RECURSE source_cpp_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/*.cpp")

target_sources(net_playrgound
  PRIVATE
  ${source_cpp_files}
)

target_link_libraries(net_playrgound
  PRIVATE
  axgl
)

include(${CMAKE_SOURCE_DIR}/_cmake/resource.cmake)
add_resource_dir(${CMAKE_CURRENT_SOURCE_DIR}/res)
