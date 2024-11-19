
function(use_resource target source_dir)
  set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/resources/${target})
  set(OUTPUT_FILE ${OUTPUT_DIR}/${source_dir})
  set(RESOURCE_TAREGET ${target}_resources_${source_dir})

  file(MAKE_DIRECTORY ${OUTPUT_DIR})

  file(GLOB_RECURSE RESORUCE_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${source_dir}/*")
  add_custom_target(
    ${RESOURCE_TAREGET}
    VERBATIM
    COMMENT "Embedding reousrce files for ${target}."
    DEPENDS ${RESORUCE_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND embedfile ${source_dir} ${OUTPUT_FILE} --namespace ${target}_${source_dir}
  )
  add_dependencies(${target} ${RESOURCE_TAREGET})

  target_sources(${target} PUBLIC ${OUTPUT_FILE}.cpp)
  set_source_files_properties(${OUTPUT_FILE}.cpp PROPERTIES GENERATED TRUE)

  return(PROPAGATE ${include_dir})
endfunction()
