
add_executable(net_test_tcp_server)

target_sources(net_test_tcp_server
  PRIVATE
  ./src/main.cpp
)

target_include_directories(net_test_tcp_server
  PRIVATE
  spdlog/include
)

target_link_libraries(net_test_tcp_server
  PRIVATE
  net
  spdlog
)
