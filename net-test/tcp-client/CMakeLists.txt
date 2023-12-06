
add_executable(net_test_tcp_client)

target_sources(net_test_tcp_client
  PRIVATE
  ./src/main.cpp
)

target_link_libraries(net_test_tcp_client
  PRIVATE
  net
)
