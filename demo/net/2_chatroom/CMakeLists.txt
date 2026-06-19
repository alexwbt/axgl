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
    axgl
    args
    ftxui::dom
    ftxui::screen
    ftxui::component
    demo_net_chatroom_fbs
  )

  if(WIN32 AND MINGW)
    target_link_libraries(demo_net_chatroom_${name} PRIVATE ws2_32 mswsock)
  endif()
endfunction()

chatroom_exec(client)
chatroom_exec(server)
