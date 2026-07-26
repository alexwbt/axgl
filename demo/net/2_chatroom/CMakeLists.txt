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

  if(WIN32 AND MINGW)
    target_link_libraries(demo_net_chatroom_${name} PRIVATE ws2_32 mswsock)
  endif()
endfunction()

chatroom_exec(client)
chatroom_exec(server)
