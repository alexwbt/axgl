
set(FBS_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated)

add_custom_target(
  bundlefile_fbs_compile
  VERBATIM
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMAND flatc --cpp -o ${FBS_OUTPUT}/fbs --filename-suffix "" bundlefile.fbs
)

add_library(bundlefile_fbs INTERFACE)
target_include_directories(bundlefile_fbs
  INTERFACE
  ${FBS_OUTPUT}
)
target_link_libraries(bundlefile_fbs
  INTERFACE
  flatbuffers
)
add_dependencies(bundlefile_fbs bundlefile_fbs_compile)

add_executable(bundlefile bundlefile.cpp)
target_link_libraries(bundlefile
  PRIVATE
  args
  spdlog
  bundlefile_fbs
)
add_dependencies(bundlefile bundlefile_fbs)
