# Header and src files.
file (GLOB SYMPL_MEMORY_HEADERS
    "${SYMPL_SOURCE_DIR}/sympl/Memory/*.hpp"
)

file (GLOB SYMPL_MEMORY_SRC
    "${SYMPL_SOURCE_DIR}/sympl/Memory/*.cpp"
)

add_library(SymplMemoryLib
    ${SYMPL_MEMORY_HEADERS} ${SYMPL_MEMORY_SRC}
)

target_link_libraries(
    SymplMemoryLib
    fmtLib
    ${SymplAppleLibs}
)

# Include our src files.
list(APPEND SYMPL_SOURCES ${SYMPL_MEMORY_HEADERS})
