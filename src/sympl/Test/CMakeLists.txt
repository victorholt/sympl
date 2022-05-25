# Header and src files.
file (GLOB SYMPL_TEST_HEADERS
    "${SYMPL_SOURCE_DIR}/sympl/Test/*.hpp"
    "${SYMPL_SOURCE_DIR}/sympl/Test/Memory/*.hpp"
)

file (GLOB SYMPL_TEST_SRC
    "${SYMPL_SOURCE_DIR}/sympl/Test/*.cpp"
    "${SYMPL_SOURCE_DIR}/sympl/Test/Memory/*.cpp"
)

add_library(SymplTestLib
    ${SYMPL_TEST_HEADERS} ${SYMPL_TEST_SRC}
)

target_link_libraries(
    SymplTestLib
    SymplMemoryLib
    fmtLib
    ${SymplAppleLibs}
)

# Include our src files.
list(APPEND SYMPL_SOURCES ${SYMPL_TEST_HEADERS})
