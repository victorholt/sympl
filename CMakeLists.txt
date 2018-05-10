cmake_minimum_required(VERSION 3.5)

# Add the windows definition if it does not exists.
if (WIN32)
    add_definitions(-D_WIN32)
elseif (APPLE)
    add_definitions(-D_APPLE)
endif ()

#set(CMAKE_C_STANDARD 11)
set (SYMPL_SHOW_CONSOLE_WINDOW TRUE)

# Compile with C++11 libraries.
if (SYMPL_SHOW_CONSOLE_WINDOW)
    if (WIN32)
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static -static-libgcc -fno-keep-inline-dllexport -std=c99")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++ -std=c++11 -fno-keep-inline-dllexport")
    else ()
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif ()
else ()
    if (WIN32)
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static -static-libgcc -fno-keep-inline-dllexport -std=c99")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++ -std=c++11 -fno-keep-inline-dllexport -mwindows")
    else ()
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif ()
endif()

# Set the base directory for the source.
set (SYMPL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Setup our common directory.
set (SYMPL_INCLUDES
    "${SYMPL_SOURCE_DIR}"
    "${SYMPL_SOURCE_DIR}/sympl/thirdparty"
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

# Setup our Sympl includes.
include_directories(${SYMPL_INCLUDES})

# Include the thirdparty libs.
include(${PROJECT_SOURCE_DIR}/src/sympl/thirdparty/fmt/CMakeLists.txt)

# Include the source.
include(${PROJECT_SOURCE_DIR}/src/sympl/CMakeLists.txt)

# Include the compiler.
include(${PROJECT_SOURCE_DIR}/src/compiler/CMakeLists.txt)

# Print out the Sympl includes.
message (STATUS "Sympl Framework Includes\n${SYMPL_INCLUDES}\n")
