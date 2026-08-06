
add_executable(compile_proxy compile_proxy.cpp)

target_link_libraries(compile_proxy
  PRIVATE
  spdlog
  args
  simdjson
  util
)
