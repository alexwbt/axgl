include(${CMAKE_SOURCE_DIR}/_cmake/resource.cmake)
include(${CMAKE_SOURCE_DIR}/_cmake/compile_proxy.cmake)

add_library(demo_net_chatroom_fbs INTERFACE)
target_link_libraries(demo_net_chatroom_fbs
  INTERFACE
  flatbuffers
)
compile_fbs(demo_net_chatroom_fbs ./fbs
  OUTPUT_FILES_OUT fbs_output_files
)
add_compile_proxy(demo_net_chatroom_fbs
  ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers
  HEADER_FILES ${fbs_output_files}
)

add_executable(demo_net_chatroom_server)
target_sources(demo_net_chatroom_server PRIVATE ./src/server.cpp)
target_link_libraries(demo_net_chatroom_server
  PRIVATE
  net
  args
  spdlog
  demo_net_chatroom_fbs
)

if(WIN32 AND MINGW)
  target_link_libraries(demo_net_chatroom_server PRIVATE ws2_32 mswsock)
endif()

add_executable(demo_net_chatroom_client
  ./src/client.cpp
  ${CMAKE_SOURCE_DIR}/_external/imgui/imgui.cpp
  ${CMAKE_SOURCE_DIR}/_external/imgui/imgui_draw.cpp
  ${CMAKE_SOURCE_DIR}/_external/imgui/imgui_tables.cpp
  ${CMAKE_SOURCE_DIR}/_external/imgui/imgui_widgets.cpp
  ${CMAKE_SOURCE_DIR}/_external/imgui/misc/cpp/imgui_stdlib.cpp
  ${CMAKE_SOURCE_DIR}/_external/imgui/backends/imgui_impl_glfw.cpp
  ${CMAKE_SOURCE_DIR}/_external/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(demo_net_chatroom_client
  PRIVATE
  ${CMAKE_SOURCE_DIR}/_external/imgui
)

target_link_libraries(demo_net_chatroom_client
  PRIVATE
  net
  args
  glfw
  glad
  demo_net_chatroom_fbs
)

if(WIN32 AND MINGW)
  target_link_libraries(demo_net_chatroom_client PRIVATE ws2_32 mswsock)
endif()
