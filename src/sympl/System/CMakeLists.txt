# Header and src files.
file (GLOB SYMPL_CORE_HEADERS
    "${SYMPL_SOURCE_DIR}/sympl/System/*.hpp"
)

file (GLOB SYMPL_CORE_SRC
    "${SYMPL_SOURCE_DIR}/sympl/System/*.cpp"
)

add_library(SymplSystemLib
    ${SYMPL_CORE_HEADERS} ${SYMPL_CORE_SRC}
)

target_link_libraries(
    SymplSystemLib
    fmtLib
    ${SymplAppleLibs}
)

# Include our src files.
list(APPEND SYMPL_SOURCES ${SYMPL_CORE_HEADERS})
