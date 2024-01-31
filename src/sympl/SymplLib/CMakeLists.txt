project(sympl)

# Header and src files.
#file (GLOB SYMPL_LIB_HEADERS
#    # Core
#    "${SYMPL_SOURCE_DIR}/sympl/include/Core/*.hpp"
#
#    # Memory
#    "${SYMPL_SOURCE_DIR}/sympl/include/Memory/*.hpp"
#
#    # Parser
#    "${SYMPL_SOURCE_DIR}/sympl/include/Parser/*.hpp"
#    "${SYMPL_SOURCE_DIR}/sympl/include/Parser/Error/*.hpp"
#    "${SYMPL_SOURCE_DIR}/sympl/include/Parser/Node/*.hpp"
#    "${SYMPL_SOURCE_DIR}/sympl/include/Parser/Handle/*.hpp"
#)
file(GLOB_RECURSE SYMPL_LIB_HEADERS "${SYMPL_SOURCE_DIR}/sympl/*.h**")
file(GLOB_RECURSE SYMPL_LIB_SRC "${SYMPL_SOURCE_DIR}/sympl/*.c**")

add_library(sympl STATIC
    ${SYMPL_LIB_HEADERS} ${SYMPL_LIB_SRC}
)

target_link_libraries(
    sympl
    fmtLib
    ${SymplAppleLibs}
)

if (WIN32)
    set_property(TARGET sympl PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/builds/${CMAKE_BUILD_TYPE}/sympl.dll)
    set_property(TARGET sympl PROPERTY IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/builds/${CMAKE_BUILD_TYPE}/sympl.a)
else ()
    set_property(TARGET sympl PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/builds/${CMAKE_BUILD_TYPE}/sympl.dylib)
endif ()
