# Sympl Compiler Project
project(sympl_cli)

file (GLOB SYMPL_CLI_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cli/main.cpp"
)
list (APPEND SYMPL_SOURCES ${SYMPL_CLI_SRC})

# Add the sources to the executables.
add_executable(sympl_cli ${SYMPL_CLI_SRC})

target_link_libraries(
    sympl_cli
    sympl
)

# Only necessary if we've compiled as a dll.
#set_property(TARGET SymplCompiler PROPERTY COMPILE_DEFINITIONS SYMPL_IMPORTS)
