
add_executable(demo_net_tcp_ping_client)
target_sources(demo_net_tcp_ping_client PRIVATE ./src/client.cpp)
target_link_libraries(demo_net_tcp_ping_client PRIVATE net spdlog)

add_executable(demo_net_tcp_ping_server)
target_sources(demo_net_tcp_ping_server PRIVATE ./src/server.cpp)
target_link_libraries(demo_net_tcp_ping_server PRIVATE net spdlog)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_definitions(demo_net_tcp_ping_client PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
  target_compile_definitions(demo_net_tcp_ping_server PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
else()
  target_compile_definitions(demo_net_tcp_ping_client PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO)
  target_compile_definitions(demo_net_tcp_ping_server PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO)
endif()
