
add_library(axgl INTERFACE)

target_include_directories(axgl
  INTERFACE
  ./include
)

target_link_libraries(axgl
  INTERFACE
  glm
  spdlog
  cpptrace::cpptrace
  Tracy::TracyClient
)
