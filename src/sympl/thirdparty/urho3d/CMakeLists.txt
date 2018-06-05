#
# Urho3D library.
#

# Header and src files.
file (GLOB URHO3D_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sympl/thirdparty/urho3d/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sympl/thirdparty/urho3d/container/*.h"
)

file (GLOB URHO3D_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sympl/thirdparty/urho3d/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sympl/thirdparty/urho3d/container/*.cpp"
)

add_library(Urho3DLib ${URHO3D_HEADERS} ${URHO3D_SRC})

# Include our src files.
list(APPEND SYMPL_SOURCES ${URHO3D_HEADERS})

