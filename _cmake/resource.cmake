#
# Function to embed resource files into a target
# 
# This function creates a custom target that embeds resource files
# from a specified source directory into the specified target.
# It generates a corresponding source file that includes these resources.
# 
# Parameters:
# - target: The name of the target that will use the embedded resources.
# - source_dir: The directory containing the resource files to be embedded.
#
function(use_resource target source_dir)
  # Define the output directory for the resource files
  set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/resources/${target})
  set(OUTPUT_FILE ${OUTPUT_DIR}/${source_dir})
  set(RESOURCE_TARGET ${target}_resources_${source_dir})

  # Create the output directory
  file(MAKE_DIRECTORY ${OUTPUT_DIR})

  # Gather all resource files from the specified source directory
  file(GLOB_RECURSE RESOURCE_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${source_dir}/*")

  # Add custom command and target
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

  # Make specified target depends on the custom target
  add_dependencies(${target} ${RESOURCE_TARGET})

  # Add embedded resource file as source of specified target
  target_sources(${target} PRIVATE ${OUTPUT_FILE}.cpp)

  # Add output to include directories
  target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/resources)
endfunction()
