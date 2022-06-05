project(sympl)

# Header and src files.
file (GLOB SYMPL_LIB_HEADERS
    # Core
    "${SYMPL_SOURCE_DIR}/sympl/Core/*.hpp"

    # Memory
    "${SYMPL_SOURCE_DIR}/sympl/Memory/*.hpp"

    # Parser
    "${SYMPL_SOURCE_DIR}/sympl/Parser/*.hpp"
    "${SYMPL_SOURCE_DIR}/sympl/Parser/Error/*.hpp"
    "${SYMPL_SOURCE_DIR}/sympl/Parser/Node/*.hpp"
    "${SYMPL_SOURCE_DIR}/sympl/Parser/Handle/*.hpp"
)

file (GLOB SYMPL_LIB_SRC
    # Core
    "${SYMPL_SOURCE_DIR}/sympl/Core/*.cpp"

    # Memory
    "${SYMPL_SOURCE_DIR}/sympl/Memory/*.cpp"

    # Parser
    "${SYMPL_SOURCE_DIR}/sympl/Parser/*.cpp"
    "${SYMPL_SOURCE_DIR}/sympl/Parser/Error/*.cpp"
    "${SYMPL_SOURCE_DIR}/sympl/Parser/Node/*.cpp"
    "${SYMPL_SOURCE_DIR}/sympl/Parser/Handle/*.cpp"
)

add_library(sympl SHARED
    ${SYMPL_LIB_HEADERS} ${SYMPL_LIB_SRC}
)

target_link_libraries(
    sympl
    fmtLib
    ${SymplAppleLibs}
)

if (WIN32)
    set_property(TARGET SymplParserLib PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/builds/${CMAKE_BUILD_TYPE}/sympl.dll)
    set_property(TARGET SymplParserLib PROPERTY IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/builds/${CMAKE_BUILD_TYPE}/sympl.a)
else ()
    set_property(TARGET SymplParserLib PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/builds/${CMAKE_BUILD_TYPE}/sympl.dylib)
endif ()
