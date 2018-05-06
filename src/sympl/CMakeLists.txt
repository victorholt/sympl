# Header and src files.
file (GLOB SYMPL_FRAMEWORK_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sympl/core/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sympl/io/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sympl/util/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sympl/script/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sympl/script/keywords/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sympl/script/methods/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sympl/thirdparty/*.h"
)

file (GLOB SYMPL_FRAMEWORK_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sympl/core/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sympl/io/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sympl/util/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sympl/script/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sympl/script/keywords/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sympl/script/methods/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sympl/thirdparty/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sympl/thirdparty/*.c"
)

add_library(SymplLib
    ${SYMPL_FRAMEWORK_HEADERS} ${SYMPL_FRAMEWORK_SRC}
)

target_link_libraries(
    SymplLib
    fmtLib
)

# Include our src files.
list(APPEND SYMPL_SOURCES ${SYMPL_FRAMEWORK_HEADERS})
