# Header and src files.
file (GLOB SYMPL_FRAMEWORK_HEADERS
    "${SYMPL_SOURCE_DIR}/sympl/script/*.h"
)

file (GLOB SYMPL_FRAMEWORK_SRC
    "${SYMPL_SOURCE_DIR}/sympl/script/*.cpp"
)

add_library(SymplScriptLib
    ${SYMPL_FRAMEWORK_HEADERS} ${SYMPL_FRAMEWORK_SRC}
)

target_link_libraries(
        SymplScriptLib
        SymplCoreLib
        fmtLib
        ${SymplAppleLibs}
)

# Include our src files.
list(APPEND SYMPL_SOURCES ${SYMPL_FRAMEWORK_HEADERS})
