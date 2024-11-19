
add_executable(embedfile embedfile.cpp)

target_link_libraries(embedfile
  PRIVATE
  spdlog
  args
)
