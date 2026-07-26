#
# Embeds resource files (textures, fonts, etc.) into a target as compiled C++
# via the `embedfile` tool. Emits a single .cpp/.hpp pair, adds the .cpp as a
# source of `target`, and exposes the resources/ dir on the include path.
# Re-runs only when a resource file changes.
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
# Bundles resource files into a single binary blob via the `bundlefile` tool.
# Unlike embed_resource, this produces a runtime file (next to the binary)
# rather than compiled-in data.
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
# Compiles flatbuffer schemas (*.fbs) into C++ headers via `flatc --cpp`.
# Emits one <name>_fbs.h per schema into the build dir and adds it to the
# target's include path (INTERFACE scope for INTERFACE targets, PUBLIC
# otherwise).
#
function(compile_fbs target source_dir)
  cmake_parse_arguments(ARG "" "OUTPUT_FILES_OUT" "" ${ARGN})
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

  if (ARG_OUTPUT_FILES_OUT)
    set(${ARG_OUTPUT_FILES_OUT} ${OUTPUT_FILES} PARENT_SCOPE)
  endif()
endfunction()
