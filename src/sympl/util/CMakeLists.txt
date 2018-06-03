# Header and src files.
file (GLOB SYMPL_UTIL_HEADERS
    "${SYMPL_SOURCE_DIR}/sympl/thirdparty/*.h"
    "${SYMPL_SOURCE_DIR}/sympl/util/*.h"
)

file (GLOB SYMPL_UTIL_SRC
    "${SYMPL_SOURCE_DIR}/sympl/thirdparty/*.cpp"
    "${SYMPL_SOURCE_DIR}/sympl/util/*.cpp"
)

add_library(SymplUtilLib
    ${SYMPL_UTIL_HEADERS} ${SYMPL_UTIL_SRC}
)

target_link_libraries(
        SymplUtilLib
        fmtLib
        ${SymplAppleLibs}
)

# Include our src files.
list(APPEND SYMPL_SOURCES ${SYMPL_UTIL_HEADERS})
