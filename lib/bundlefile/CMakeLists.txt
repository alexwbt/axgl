include(${CMAKE_SOURCE_DIR}/_cmake/resource.cmake)

set(FBS_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated)

add_library(bundlefile_fbs INTERFACE)
target_include_directories(bundlefile_fbs
  INTERFACE
  ${FBS_OUTPUT}
)
target_link_libraries(bundlefile_fbs
  INTERFACE
  flatbuffers
)
compile_fbs(bundlefile_fbs .)

add_executable(bundlefile bundlefile.cpp)
target_link_libraries(bundlefile
  PRIVATE
  args
  spdlog
  bundlefile_fbs
)

add_library(bundlefile_lib INTERFACE)
target_include_directories(bundlefile_lib
  INTERFACE
  ./include
)
target_link_libraries(bundlefile_lib
  INTERFACE
  bundlefile_fbs
)
