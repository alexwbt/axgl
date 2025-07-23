include(${CMAKE_SOURCE_DIR}/_cmake/resource.cmake)

add_library(demo_net_chatroom_fbs INTERFACE)
target_link_libraries(demo_net_chatroom_fbs
  INTERFACE
  flatbuffers
)
compile_fbs(demo_net_chatroom_fbs ./fbs)


function(chatroom_exec name)
  add_executable(demo_net_chatroom_${name})
  target_sources(demo_net_chatroom_${name}
    PRIVATE
    ./src/${name}.cpp
  )
  target_link_libraries(demo_net_chatroom_${name}
    PRIVATE
    net
    args
    spdlog
    ftxui::dom
    ftxui::screen
    ftxui::component
    demo_net_chatroom_fbs
  )
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(demo_net_chatroom_${name} PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
  else()
    target_compile_definitions(demo_net_chatroom_${name} PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO)
  endif()
endfunction()

chatroom_exec(client)
chatroom_exec(server)
