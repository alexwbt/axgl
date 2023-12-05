
add_executable(net_test_udp_server)

target_sources(net_test_udp_server
  PRIVATE
  ./src/main.cpp
)

target_include_directories(net_test_udp_server
  PRIVATE
  spdlog/include
)

target_link_libraries(net_test_udp_server
  PRIVATE
  net
  spdlog
)
