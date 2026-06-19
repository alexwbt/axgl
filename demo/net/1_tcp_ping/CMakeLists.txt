
add_executable(demo_net_tcp_ping_client)
target_sources(demo_net_tcp_ping_client PRIVATE ./src/client.cpp)
target_link_libraries(demo_net_tcp_ping_client PRIVATE axgl net)

add_executable(demo_net_tcp_ping_server)
target_sources(demo_net_tcp_ping_server PRIVATE ./src/server.cpp)
target_link_libraries(demo_net_tcp_ping_server PRIVATE axgl net)

if(WIN32 AND MINGW)
  target_link_libraries(demo_net_tcp_ping_client PRIVATE ws2_32 mswsock)
  target_link_libraries(demo_net_tcp_ping_server PRIVATE ws2_32 mswsock)
endif()
