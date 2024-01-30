project(sympl)

# Header and src files.
file (GLOB_RECURSE SYMPL_LIB_HEADERS ${SYMPL_SOURCE_DIR}/sympl/*.h**)
file (GLOB_RECURSE SYMPL_LIB_SRC ${SYMPL_SOURCE_DIR}/sympl/*.c**)

add_library(sympl STATIC
    ${SYMPL_LIB_HEADERS} ${SYMPL_LIB_SRC}
)

target_link_libraries(
    sympl
    ${SymplAppleLibs}
)

if (WIN32)
    set_property(TARGET sympl PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/builds/${CMAKE_BUILD_TYPE}/sympl.dll)
    set_property(TARGET sympl PROPERTY IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/builds/${CMAKE_BUILD_TYPE}/sympl.a)
else ()
    set_property(TARGET sympl PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/builds/${CMAKE_BUILD_TYPE}/sympl.dylib)
endif ()
