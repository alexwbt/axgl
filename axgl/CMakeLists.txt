
add_library(axgl INTERFACE
  include/axgl/interface/domain_objects/pointer.hpp)

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
