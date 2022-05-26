# Header and src files.
file (GLOB SYMPL_PARSER_HEADERS
    "${SYMPL_SOURCE_DIR}/sympl/Parser/*.hpp"
    "${SYMPL_SOURCE_DIR}/sympl/Parser/Error/*.hpp"
    "${SYMPL_SOURCE_DIR}/sympl/Parser/Node/*.hpp"
    "${SYMPL_SOURCE_DIR}/sympl/Parser/Handle/*.hpp"
)

file (GLOB SYMPL_PARSER_SRC
    "${SYMPL_SOURCE_DIR}/sympl/Parser/*.cpp"
    "${SYMPL_SOURCE_DIR}/sympl/Parser/Error/*.cpp"
    "${SYMPL_SOURCE_DIR}/sympl/Parser/Node/*.cpp"
    "${SYMPL_SOURCE_DIR}/sympl/Parser/Handle/*.cpp"
)

add_library(SymplParserLib
    ${SYMPL_PARSER_HEADERS} ${SYMPL_PARSER_SRC}
)

target_link_libraries(
    SymplParserLib
    SymplCoreLib
    fmtLib
    ${SymplAppleLibs}
)

# Include our src files.
list(APPEND SYMPL_SOURCES ${SYMPL_PARSER_HEADERS})
