
add_executable(net_test_udp_client)

target_sources(net_test_udp_client
  PRIVATE
  ./src/main.cpp
)

target_include_directories(net_test_udp_client
  PRIVATE
  spdlog/include
)

target_link_libraries(net_test_udp_client
  PRIVATE
  net
  spdlog
)
