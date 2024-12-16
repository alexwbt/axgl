include(${CMAKE_SOURCE_DIR}/_cmake/resource.cmake)

add_executable(bundlefile bundlefile.cpp)
target_link_libraries(bundlefile
  PRIVATE
  args
  spdlog
  flatbuffers
)
compile_fbs(bundlefile .)

add_library(bundlefile_lib INTERFACE)
target_include_directories(bundlefile_lib
  INTERFACE
  ./include
  ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers
)
target_link_libraries(bundlefile_lib
  INTERFACE
  flatbuffers
  bundlefile_fbs
)
