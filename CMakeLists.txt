cmake_minimum_required(VERSION 3.5)

# Add the windows definition if it does not exists.
if (WIN32)
    add_definitions(-D_WIN32)
elseif (APPLE)
    add_definitions(-D_APPLE)
endif ()

#set(CMAKE_C_STANDARD 11)
set (SYMPL_SHOW_CONSOLE_WINDOW TRUE)

if(NOT "${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
    message("You are on MSVC")
    add_definitions(-D_MSVC)
endif()

# Compile with C++11 libraries.
if (SYMPL_SHOW_CONSOLE_WINDOW)
    if (WIN32)
        if (NOT "${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
            set(CMAKE_CXX_STANDARD 17)
        else ()
            set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static -static-libgcc -fno-keep-inline-dllexport -std=c99 -fno-exceptions -fno-unwind-tables")
            set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++ -std=c++17 -fno-keep-inline-dllexport -fno-exceptions -fno-unwind-tables")
        endif ()
    else ()
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fno-exceptions -fno-unwind-tables")
    endif ()
else ()
    if (WIN32)
        if (NOT "${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
            add_definitions(-D_WIN32_DISABLE_CONSOLE)
            set(CMAKE_CXX_STANDARD 17)
        else ()
            set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static -static-libgcc -fno-keep-inline-dllexport -std=c99 -fno-exceptions -fno-unwind-tables")
            set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++ -std=c++17 -fno-keep-inline-dllexport -fno-exceptions -fno-unwind-tables")
        endif ()
    else ()
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fno-exceptions -fno-unwind-tables")
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
    add_definitions(-DSYMPL_DEBUG)
    set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/builds/debug)
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${PROJECT_SOURCE_DIR}/builds/debug)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/builds/debug)
else ()
    set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/builds/release)
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${PROJECT_SOURCE_DIR}/builds/release)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/builds/release)
endif ()

# Apple-specific libraries.
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

# Setup our Sympl includes.
include_directories(${SYMPL_INCLUDES})

# Include the thirdparty libs.
include(${CMAKE_CURRENT_SOURCE_DIR}/src/sympl/thirdparty/fmt/CMakeLists.txt)

# Include the source.
include(src/sympl/SymplLib/CMakeLists.txt)

# Include the compiler.
include(${CMAKE_CURRENT_SOURCE_DIR}/compiler/CMakeLists.txt)

# Print out the Sympl includes.
message (STATUS "Sympl Framework Includes\n${SYMPL_INCLUDES}\n")
