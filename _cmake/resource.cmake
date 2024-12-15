#
# Function to embed resource files into a target
#
function(embed_resource target source_dir)
  set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/resources/${target})
  set(OUTPUT_FILE ${OUTPUT_DIR}/${source_dir})
  set(RESOURCE_TARGET ${target}_resources_${source_dir})

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
  set(RESOURCE_TARGET ${target}_resources_${source_dir})

  file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
  file(GLOB_RECURSE RESOURCE_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${source_dir}/*")

  add_custom_command(
    COMMAND bundlefile ${source_dir} ${OUTPUT_FILE}
    COMMENT "Bundling resource files for ${target} to ${OUTPUT_FILE}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS embedfile ${RESOURCE_FILES}
    OUTPUT ${OUTPUT_FILE}
  )
  add_custom_target(
    ${RESOURCE_TARGET} ALL
    DEPENDS ${OUTPUT_FILE}
  )
  add_dependencies(${target} ${RESOURCE_TARGET})
endfunction()
