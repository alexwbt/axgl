
add_executable(flat_test)

target_sources(flat_test
  PRIVATE
  ./src/main.cpp
)

target_link_libraries(flat_test
  PRIVATE
  net
)
