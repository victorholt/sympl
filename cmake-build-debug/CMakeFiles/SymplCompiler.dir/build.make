# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = C:\Users\VictorGaming\AppData\Local\JetBrains\Toolbox\apps\CLion\ch-0\181.4203.549\bin\cmake\bin\cmake.exe

# The command to remove a file.
RM = C:\Users\VictorGaming\AppData\Local\JetBrains\Toolbox\apps\CLion\ch-0\181.4203.549\bin\cmake\bin\cmake.exe -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\Projects\sympl

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\Projects\sympl\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/SymplCompiler.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/SymplCompiler.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/SymplCompiler.dir/flags.make

CMakeFiles/SymplCompiler.dir/src/compiler/main.c.obj: CMakeFiles/SymplCompiler.dir/flags.make
CMakeFiles/SymplCompiler.dir/src/compiler/main.c.obj: ../src/compiler/main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\Projects\sympl\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/SymplCompiler.dir/src/compiler/main.c.obj"
	D:\MingGW\x86_64-6.2.0-posix-seh-rt_v5-rev1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\SymplCompiler.dir\src\compiler\main.c.obj   -c D:\Projects\sympl\src\compiler\main.c

CMakeFiles/SymplCompiler.dir/src/compiler/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SymplCompiler.dir/src/compiler/main.c.i"
	D:\MingGW\x86_64-6.2.0-posix-seh-rt_v5-rev1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\Projects\sympl\src\compiler\main.c > CMakeFiles\SymplCompiler.dir\src\compiler\main.c.i

CMakeFiles/SymplCompiler.dir/src/compiler/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SymplCompiler.dir/src/compiler/main.c.s"
	D:\MingGW\x86_64-6.2.0-posix-seh-rt_v5-rev1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S D:\Projects\sympl\src\compiler\main.c -o CMakeFiles\SymplCompiler.dir\src\compiler\main.c.s

CMakeFiles/SymplCompiler.dir/src/compiler/main.c.obj.requires:

.PHONY : CMakeFiles/SymplCompiler.dir/src/compiler/main.c.obj.requires

CMakeFiles/SymplCompiler.dir/src/compiler/main.c.obj.provides: CMakeFiles/SymplCompiler.dir/src/compiler/main.c.obj.requires
	$(MAKE) -f CMakeFiles\SymplCompiler.dir\build.make CMakeFiles/SymplCompiler.dir/src/compiler/main.c.obj.provides.build
.PHONY : CMakeFiles/SymplCompiler.dir/src/compiler/main.c.obj.provides

CMakeFiles/SymplCompiler.dir/src/compiler/main.c.obj.provides.build: CMakeFiles/SymplCompiler.dir/src/compiler/main.c.obj


# Object files for target SymplCompiler
SymplCompiler_OBJECTS = \
"CMakeFiles/SymplCompiler.dir/src/compiler/main.c.obj"

# External object files for target SymplCompiler
SymplCompiler_EXTERNAL_OBJECTS =

../builds/debug/SymplCompiler.exe: CMakeFiles/SymplCompiler.dir/src/compiler/main.c.obj
../builds/debug/SymplCompiler.exe: CMakeFiles/SymplCompiler.dir/build.make
../builds/debug/SymplCompiler.exe: ../builds/debug/libSymplLib.a
../builds/debug/SymplCompiler.exe: CMakeFiles/SymplCompiler.dir/linklibs.rsp
../builds/debug/SymplCompiler.exe: CMakeFiles/SymplCompiler.dir/objects1.rsp
../builds/debug/SymplCompiler.exe: CMakeFiles/SymplCompiler.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\Projects\sympl\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable ..\builds\debug\SymplCompiler.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\SymplCompiler.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/SymplCompiler.dir/build: ../builds/debug/SymplCompiler.exe

.PHONY : CMakeFiles/SymplCompiler.dir/build

CMakeFiles/SymplCompiler.dir/requires: CMakeFiles/SymplCompiler.dir/src/compiler/main.c.obj.requires

.PHONY : CMakeFiles/SymplCompiler.dir/requires

CMakeFiles/SymplCompiler.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\SymplCompiler.dir\cmake_clean.cmake
.PHONY : CMakeFiles/SymplCompiler.dir/clean

CMakeFiles/SymplCompiler.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\Projects\sympl D:\Projects\sympl D:\Projects\sympl\cmake-build-debug D:\Projects\sympl\cmake-build-debug D:\Projects\sympl\cmake-build-debug\CMakeFiles\SymplCompiler.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/SymplCompiler.dir/depend

