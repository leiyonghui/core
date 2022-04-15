
aux_source_directory(./ CORE_SRCS)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/libs)
add_library(core ${CORE_SRCS})

#message(status "project sdir: .....${PROJECT_SOURCE_DIR}")
#message(status "PROJECT_BINARY_DIR: .....${PROJECT_BINARY_DIR}")
#message(status "LIBRARY_OUTPUT_PATH: .....${LIBRARY_OUTPUT_PATH}")

