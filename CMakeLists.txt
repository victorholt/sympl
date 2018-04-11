cmake_minimum_required(VERSION 3.10)

# Add the windows definition if it does not exists.
if (WIN32)
    add_definitions(-D_WIN32)
elseif (APPLE)
    add_definitions(-D_APPLE)
endif ()

set(CMAKE_C_STANDARD 11)

# Set the base directory for the source.
set (SYMPL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Setup our common directory.
set (SYMPL_INCLUDES
    "${SYMPL_SOURCE_DIR}"
)
set (SYMPL_SOURCES)

# Setup the build locations.
if (CMAKE_BUILD_TYPE MATCHES Debug)
    set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/builds/debug)
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${PROJECT_SOURCE_DIR}/builds/debug)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/builds/debug)
else ()
    set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/builds/release)
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${PROJECT_SOURCE_DIR}/builds/release)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/builds/release)
endif ()

# Include the source.
include(src/sympl/CMakeLists.txt)

# Include the compiler.
include(src/compiler/CMakeLists.txt)

# Setup our Sympl includes.
include_directories(${SYMPL_INCLUDES})
