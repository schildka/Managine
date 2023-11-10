set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_BUILTIN_STL_SUPPORT YES)

doxygen_add_docs(gl3_engine_docs
    "${PROJECT_SOURCE_DIR}/engine/include"
    WORKING_DIRECTORY
    "${PROJECT_SOURCE_DIR}/engine/include"
)
