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
# The function:
# 1. Creates an output directory for the resources.
# 2. Gathers resource files from the specified source directory.
# 3. Defines a custom target that embeds these resources.
# 4. Ensures the specified target depends on the custom target.
# 5. Adds the generated source file to the target and marks it as generated.
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

  # Define a custom target to embed the resource files
  add_custom_target(
    ${RESOURCE_TARGET}
    VERBATIM
    COMMENT "Embedding resource files for ${target}."
    DEPENDS ${RESOURCE_FILES}  # Ensure the target depends on all resource files
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND embedfile ${source_dir} ${OUTPUT_FILE} --namespace ${target}_${source_dir}
  )

  # Ensure the main target depends on the custom resource target
  add_dependencies(${target} ${RESOURCE_TARGET})

  # Add the generated source file to the target
  target_sources(${target} PRIVATE ${OUTPUT_FILE}.cpp)
  
  # Mark the generated source file to prevent recompilation
  set_source_files_properties(${OUTPUT_FILE}.cpp PROPERTIES GENERATED TRUE)
endfunction()
