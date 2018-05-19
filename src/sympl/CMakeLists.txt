# Header and src files.
file (GLOB SYMPL_FRAMEWORK_HEADERS
    "${SYMPL_SOURCE_DIR}/sympl/core/*.h"
    "${SYMPL_SOURCE_DIR}/sympl/io/*.h"
    "${SYMPL_SOURCE_DIR}/sympl/util/*.h"
    "${SYMPL_SOURCE_DIR}/sympl/script/*.h"
    "${SYMPL_SOURCE_DIR}/sympl/script/keywords/*.h"
    "${SYMPL_SOURCE_DIR}/sympl/script/methods/*.h"
    "${SYMPL_SOURCE_DIR}/sympl/thirdparty/*.h"
)

file (GLOB SYMPL_FRAMEWORK_SRC
    "${SYMPL_SOURCE_DIR}/sympl/core/*.cpp"
    "${SYMPL_SOURCE_DIR}/sympl/io/*.cpp"
    "${SYMPL_SOURCE_DIR}/sympl/util/*.cpp"
    "${SYMPL_SOURCE_DIR}/sympl/script/*.cpp"
    "${SYMPL_SOURCE_DIR}/sympl/script/keywords/*.cpp"
    "${SYMPL_SOURCE_DIR}/sympl/script/methods/*.cpp"
    "${SYMPL_SOURCE_DIR}/sympl/thirdparty/*.cpp"
    "${SYMPL_SOURCE_DIR}/sympl/thirdparty/*.c"
)

add_library(SymplLib
    ${SYMPL_FRAMEWORK_HEADERS} ${SYMPL_FRAMEWORK_SRC}
)

list (APPEND SymplAppleLibs "")
if (APPLE)
    if (IOS OR TVOS)
        list (APPEND SymplAppleLibs
                "-framework Foundation"
                "-framework QuartzCore"
        )
    else ()
        list (APPEND SymplAppleLibs
                "-framework CoreFoundation"
                "-framework SystemConfiguration"
                "-framework CoreServices"
        )
    endif ()
endif ()

target_link_libraries(
    SymplLib
    fmtLib
    ${SymplAppleLibs}
)

# Include our src files.
list(APPEND SYMPL_SOURCES ${SYMPL_FRAMEWORK_HEADERS})
