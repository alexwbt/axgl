include(${CMAKE_SOURCE_DIR}/_cmake/resource.cmake)
include(${CMAKE_SOURCE_DIR}/_cmake/compile_proxy.cmake)

add_executable(bundlefile bundlefile.cpp)
target_link_libraries(bundlefile
  PRIVATE
  args
  spdlog
  flatbuffers
)
compile_fbs(bundlefile .
  OUTPUT_FILES_OUT fbs_output_files
)

add_library(bundlefile_lib INTERFACE)
target_include_directories(bundlefile_lib
  INTERFACE
  ./include
  ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers
)
target_link_libraries(bundlefile_lib
  INTERFACE
  flatbuffers
)
add_compile_proxy(bundlefile_lib
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  HEADER_FILES ${fbs_output_files}
)
