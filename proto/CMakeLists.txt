
add_library(proto INTERFACE)

file(GLOB_RECURSE FLATBUFFER_SCHEMA_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/*.fbs")

set(PROTO_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated)

add_custom_target(
    build_flatbuffer_schema
    VERBATIM
    COMMENT "Building flatbuffer schemas"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND flatc --cpp -o ${PROTO_OUTPUT}/proto --filename-suffix "" ${FLATBUFFER_SCHEMA_FILES}
)
add_dependencies(proto build_flatbuffer_schema)

target_include_directories(proto
    INTERFACE
    ${PROTO_OUTPUT}
)

target_link_libraries(proto
    INTERFACE
    flatbuffers
)

