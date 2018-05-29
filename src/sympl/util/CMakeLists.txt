# Header and src files.
file (GLOB SYMPL_FRAMEWORK_HEADERS
        "${SYMPL_SOURCE_DIR}/sympl/util/*.h"
        )

file (GLOB SYMPL_FRAMEWORK_SRC
        "${SYMPL_SOURCE_DIR}/sympl/util/*.cpp"
        )

add_library(SymplUtilLib
        ${SYMPL_FRAMEWORK_HEADERS} ${SYMPL_FRAMEWORK_SRC}
        )

target_link_libraries(
        SymplUtilLib
        fmtLib
        ${SymplAppleLibs}
)

# Include our src files.
list(APPEND SYMPL_SOURCES ${SYMPL_FRAMEWORK_HEADERS})
