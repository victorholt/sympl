# Header and src files.
file (GLOB SYMPL_HEADERS
    "${SYMPL_SOURCE_DIR}/sympl/core/*.h"
    "${SYMPL_SOURCE_DIR}/sympl/io/*.h"
    "${SYMPL_SOURCE_DIR}/sympl/util/*.h"
    "${SYMPL_SOURCE_DIR}/sympl/script/*.h"
    "${SYMPL_SOURCE_DIR}/sympl/thirdparty/*.h"
)

file (GLOB SYMPL_SRC
    "${SYMPL_SOURCE_DIR}/sympl/core/*.cpp"
    "${SYMPL_SOURCE_DIR}/sympl/io/*.cpp"
    "${SYMPL_SOURCE_DIR}/sympl/util/*.cpp"
    "${SYMPL_SOURCE_DIR}/sympl/script/*.cpp"
    "${SYMPL_SOURCE_DIR}/sympl/thirdparty/*.cpp"
    "${SYMPL_SOURCE_DIR}/sympl/thirdparty/*.c"
)

add_library(SymplLib
    ${SYMPL_HEADERS} ${SYMPL_SRC}
)

target_link_libraries(
    SymplLib
    fmtLib
)

# Include our src files.
list(APPEND SYMPL_SOURCES ${SYMPL_HEADERS})
