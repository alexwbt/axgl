
set(RESOURCE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res)

function(add_resource_dir FOLDER)
    file(GLOB files "${FOLDER}/*")

    foreach(file ${files})
        file(COPY ${file} DESTINATION ${RESOURCE_RUNTIME_OUTPUT_DIRECTORY})
    endforeach()
endfunction()
