#
# Function to embed resource files into a target
#
function(embed_resource target source_dir)
  set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/resources/${target})
  set(OUTPUT_FILE ${OUTPUT_DIR}/${source_dir})
  set(RESOURCE_TARGET ${target}_resources)

  file(MAKE_DIRECTORY ${OUTPUT_DIR})
  file(GLOB_RECURSE RESOURCE_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${source_dir}/*")

  add_custom_command(
    COMMAND embedfile ${source_dir} ${OUTPUT_FILE} --namespace ${target}_${source_dir}
    COMMENT "Embedding resource files for ${target}."
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT ${OUTPUT_FILE}.cpp ${OUTPUT_FILE}.hpp
    DEPENDS embedfile ${RESOURCE_FILES}
  )
  add_custom_target(
    ${RESOURCE_TARGET} ALL
    DEPENDS ${OUTPUT_FILE}.cpp ${OUTPUT_FILE}.hpp
  )
  add_dependencies(${target} ${RESOURCE_TARGET})
  target_sources(${target} PRIVATE ${OUTPUT_FILE}.cpp)
  target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/resources)
endfunction()

#
# Function to bundle resource files into a binary file
#
function(bundle_resource target source_dir)
  set(OUTPUT_FILE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${target}_${source_dir}.bin)
  set(RESOURCE_TARGET ${target}_resources)

  file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
  file(GLOB_RECURSE RESOURCE_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${source_dir}/*")

  add_custom_command(
    COMMAND bundlefile ${source_dir} ${OUTPUT_FILE}
    COMMENT "Bundling resource files for ${target} to ${OUTPUT_FILE}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS bundlefile ${RESOURCE_FILES}
    OUTPUT ${OUTPUT_FILE}
  )
  add_custom_target(
    ${RESOURCE_TARGET} ALL
    DEPENDS ${OUTPUT_FILE}
  )
  add_dependencies(${target} ${RESOURCE_TARGET})
endfunction()

#
# Function to compile flatbuffers
#
function(compile_fbs target source_dir)
  set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers/${target})
  set(FBS_TARGET ${target}_fbs)

  file(GLOB_RECURSE FBS_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${source_dir}/*.fbs")

  set(OUTPUT_FILES)
  foreach(item IN LISTS FBS_FILES)
    get_filename_component(output_header "${item}" NAME_WLE)
    list(APPEND OUTPUT_FILES "${OUTPUT_DIR}/${output_header}_fbs.h")
  endforeach()

  add_custom_command(
    COMMAND flatc --cpp -o ${OUTPUT_DIR} --filename-suffix "_fbs" ${FBS_FILES}
    COMMENT "Compiling flatbuffers for ${target} to ${OUTPUT_DIR}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS flatbuffers flatc ${FBS_FILES}
    OUTPUT ${OUTPUT_FILES}
  )
  add_custom_target(
    ${FBS_TARGET} ALL
    DEPENDS ${OUTPUT_FILES}
  )
  add_dependencies(${target} ${FBS_TARGET})
  get_target_property(target_type ${target} TYPE)
  if(target_type STREQUAL "INTERFACE_LIBRARY")
    target_include_directories(${target} INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers)
  else()
    target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers)
  endif()

  #
  # Generate proxy library
  #
  # flatc emits header-only *_fbs.h files with no .cpp companions, so they
  # produce no compile commands of their own. Without a TU that includes
  # them, clangd (and clang-tidy) cannot resolve includes or surface
  # diagnostics on the generated headers. Emit one synthetic .cpp per
  # *_fbs.h, add them to a Debug-only EXCLUDE_FROM_ALL static library
  # linked to ${target} (and gated on the FBS generation), so the Debug
  # build produces per-header compile commands in compile_commands.json
  # without affecting the release build or linking anything.
  #
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(PROXY_SOURCES)

    foreach(output IN LISTS OUTPUT_FILES)
      file(RELATIVE_PATH header_relative "${CMAKE_CURRENT_BINARY_DIR}/flatbuffers" "${output}")
      set(proxy_source "${CMAKE_CURRENT_BINARY_DIR}/flatbuffer_sources/${header_relative}.cpp")
      file(GENERATE OUTPUT "${proxy_source}"
        CONTENT "#include <${header_relative}>\n"
      )
      list(APPEND PROXY_SOURCES "${proxy_source}")
    endforeach()

    if(PROXY_SOURCES)
      add_library(${target}_fbs_proxy STATIC EXCLUDE_FROM_ALL ${PROXY_SOURCES})
      target_include_directories(${target}_fbs_proxy PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers)
      target_link_libraries(${target}_fbs_proxy PRIVATE flatbuffers)
      add_dependencies(${target}_fbs_proxy ${FBS_TARGET})
    endif()
  endif()
endfunction()
