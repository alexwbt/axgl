
add_executable(net_playrgound_server)

file(GLOB_RECURSE source_cpp_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/*.cpp")

target_sources(net_playrgound_server
  PRIVATE
  ${source_cpp_files}
)

target_link_libraries(net_playrgound_server
  PRIVATE
  net_playground_common
)
