
add_executable(assbin assbin.cpp)

target_link_libraries(assbin
  PRIVATE
  args
  spdlog
  assimp
)
