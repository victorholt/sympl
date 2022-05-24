# Sympl Compiler Project
project(SymplCompiler)

file (GLOB SYMPL_COMPILER_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/compiler/main.cpp"
)
list (APPEND SYMPL_SOURCES ${SYMPL_COMPILER_SRC})

# Add the sources to the executables.
add_executable(SymplCompiler ${SYMPL_COMPILER_SRC})

target_link_libraries(
    SymplCompiler
    SymplSystemLib
    fmtLib
)

# Only necessary if we've compiled as a dll.
#set_property(TARGET SymplCompiler PROPERTY COMPILE_DEFINITIONS SYMPL_IMPORTS)
