#
# CppFormat library.
#

# Header and src files.
file (GLOB CPPFORMAT_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sympl/thirdparty/fmt/*.h"
)

file (GLOB CPPFORMAT_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sympl/thirdparty/fmt/*.cc"
)

add_library(fmtLib ${CPPFORMAT_HEADERS} ${CPPFORMAT_SRC})

# Include our src files.
list(APPEND SYMPL_SOURCES ${CPPFORMAT_HEADERS})

