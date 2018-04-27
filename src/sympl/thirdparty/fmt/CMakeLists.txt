#
# CppFormat library.
#

# Header and src files.
file (GLOB CPPFORMAT_HEADERS
    "${SYMPL_SOURCE_DIR}/sympl/thirdparty/fmt/*.h"
)

file (GLOB CPPFORMAT_SRC
    "${SYMPL_SOURCE_DIR}/sympl/thirdparty/fmt/*.cc"
)

add_library(fmtLib ${CPPFORMAT_HEADERS} ${CPPFORMAT_SRC})

# Include our src files.
list(APPEND SYMPL_SOURCES ${CPPFORMAT_HEADERS})

