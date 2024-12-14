
add_executable(bundlefile bundlefile.cpp)

target_link_libraries(bundlefile
  PRIVATE
  spdlog
  args
)
