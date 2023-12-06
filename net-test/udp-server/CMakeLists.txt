
add_executable(net_test_udp_server)

target_sources(net_test_udp_server
  PRIVATE
  ./src/main.cpp
)

target_link_libraries(net_test_udp_server
  PRIVATE
  net
)
