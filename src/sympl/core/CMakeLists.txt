# Header and src files.
file (GLOB SYMPL_CORE_HEADERS
    "${SYMPL_SOURCE_DIR}/sympl/core/*.h"
)

file (GLOB SYMPL_CORE_SRC
    "${SYMPL_SOURCE_DIR}/sympl/core/*.cpp"
)

add_library(SymplCoreLib
    ${SYMPL_CORE_HEADERS} ${SYMPL_CORE_SRC}
)

target_link_libraries(
        SymplCoreLib
        fmtLib
        Urho3DLib
        ${SymplAppleLibs}
)

# Include our src files.
list(APPEND SYMPL_SOURCES ${SYMPL_CORE_HEADERS})
