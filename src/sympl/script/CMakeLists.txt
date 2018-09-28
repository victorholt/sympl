# Header and src files.
file (GLOB SYMPL_SCRIPT_HEADERS
    "${SYMPL_SOURCE_DIR}/sympl/script/*.h"
    "${SYMPL_SOURCE_DIR}/sympl/script/methods/*.h"
    "${SYMPL_SOURCE_DIR}/sympl/script/resolvers/*.h"
)

file (GLOB SYMPL_SCRIPT_SRC
    "${SYMPL_SOURCE_DIR}/sympl/script/*.cpp"
    "${SYMPL_SOURCE_DIR}/sympl/script/methods/*.cpp"
    "${SYMPL_SOURCE_DIR}/sympl/script/resolvers/*.cpp"
)

add_library(SymplScriptLib
    ${SYMPL_SCRIPT_HEADERS} ${SYMPL_SCRIPT_SRC}
)

target_link_libraries(
        SymplScriptLib
        SymplCoreLib
        SymplUtilLib
        fmtLib
        Urho3DLib
        ${SymplAppleLibs}
)

# Include our src files.
list(APPEND SYMPL_SOURCES ${SYMPL_SCRIPT_HEADERS})
