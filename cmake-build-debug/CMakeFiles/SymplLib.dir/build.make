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
include CMakeFiles/SymplLib.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/SymplLib.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/SymplLib.dir/flags.make

CMakeFiles/SymplLib.dir/src/sympl/core/alloc.c.obj: CMakeFiles/SymplLib.dir/flags.make
CMakeFiles/SymplLib.dir/src/sympl/core/alloc.c.obj: ../src/sympl/core/alloc.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\Projects\sympl\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/SymplLib.dir/src/sympl/core/alloc.c.obj"
	D:\MingGW\x86_64-6.2.0-posix-seh-rt_v5-rev1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\SymplLib.dir\src\sympl\core\alloc.c.obj   -c D:\Projects\sympl\src\sympl\core\alloc.c

CMakeFiles/SymplLib.dir/src/sympl/core/alloc.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SymplLib.dir/src/sympl/core/alloc.c.i"
	D:\MingGW\x86_64-6.2.0-posix-seh-rt_v5-rev1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\Projects\sympl\src\sympl\core\alloc.c > CMakeFiles\SymplLib.dir\src\sympl\core\alloc.c.i

CMakeFiles/SymplLib.dir/src/sympl/core/alloc.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SymplLib.dir/src/sympl/core/alloc.c.s"
	D:\MingGW\x86_64-6.2.0-posix-seh-rt_v5-rev1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S D:\Projects\sympl\src\sympl\core\alloc.c -o CMakeFiles\SymplLib.dir\src\sympl\core\alloc.c.s

CMakeFiles/SymplLib.dir/src/sympl/core/alloc.c.obj.requires:

.PHONY : CMakeFiles/SymplLib.dir/src/sympl/core/alloc.c.obj.requires

CMakeFiles/SymplLib.dir/src/sympl/core/alloc.c.obj.provides: CMakeFiles/SymplLib.dir/src/sympl/core/alloc.c.obj.requires
	$(MAKE) -f CMakeFiles\SymplLib.dir\build.make CMakeFiles/SymplLib.dir/src/sympl/core/alloc.c.obj.provides.build
.PHONY : CMakeFiles/SymplLib.dir/src/sympl/core/alloc.c.obj.provides

CMakeFiles/SymplLib.dir/src/sympl/core/alloc.c.obj.provides.build: CMakeFiles/SymplLib.dir/src/sympl/core/alloc.c.obj


CMakeFiles/SymplLib.dir/src/sympl/core/resource.c.obj: CMakeFiles/SymplLib.dir/flags.make
CMakeFiles/SymplLib.dir/src/sympl/core/resource.c.obj: ../src/sympl/core/resource.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\Projects\sympl\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/SymplLib.dir/src/sympl/core/resource.c.obj"
	D:\MingGW\x86_64-6.2.0-posix-seh-rt_v5-rev1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\SymplLib.dir\src\sympl\core\resource.c.obj   -c D:\Projects\sympl\src\sympl\core\resource.c

CMakeFiles/SymplLib.dir/src/sympl/core/resource.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SymplLib.dir/src/sympl/core/resource.c.i"
	D:\MingGW\x86_64-6.2.0-posix-seh-rt_v5-rev1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\Projects\sympl\src\sympl\core\resource.c > CMakeFiles\SymplLib.dir\src\sympl\core\resource.c.i

CMakeFiles/SymplLib.dir/src/sympl/core/resource.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SymplLib.dir/src/sympl/core/resource.c.s"
	D:\MingGW\x86_64-6.2.0-posix-seh-rt_v5-rev1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S D:\Projects\sympl\src\sympl\core\resource.c -o CMakeFiles\SymplLib.dir\src\sympl\core\resource.c.s

CMakeFiles/SymplLib.dir/src/sympl/core/resource.c.obj.requires:

.PHONY : CMakeFiles/SymplLib.dir/src/sympl/core/resource.c.obj.requires

CMakeFiles/SymplLib.dir/src/sympl/core/resource.c.obj.provides: CMakeFiles/SymplLib.dir/src/sympl/core/resource.c.obj.requires
	$(MAKE) -f CMakeFiles\SymplLib.dir\build.make CMakeFiles/SymplLib.dir/src/sympl/core/resource.c.obj.provides.build
.PHONY : CMakeFiles/SymplLib.dir/src/sympl/core/resource.c.obj.provides

CMakeFiles/SymplLib.dir/src/sympl/core/resource.c.obj.provides.build: CMakeFiles/SymplLib.dir/src/sympl/core/resource.c.obj


CMakeFiles/SymplLib.dir/src/sympl/core/string_buffer.c.obj: CMakeFiles/SymplLib.dir/flags.make
CMakeFiles/SymplLib.dir/src/sympl/core/string_buffer.c.obj: ../src/sympl/core/string_buffer.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\Projects\sympl\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/SymplLib.dir/src/sympl/core/string_buffer.c.obj"
	D:\MingGW\x86_64-6.2.0-posix-seh-rt_v5-rev1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\SymplLib.dir\src\sympl\core\string_buffer.c.obj   -c D:\Projects\sympl\src\sympl\core\string_buffer.c

CMakeFiles/SymplLib.dir/src/sympl/core/string_buffer.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SymplLib.dir/src/sympl/core/string_buffer.c.i"
	D:\MingGW\x86_64-6.2.0-posix-seh-rt_v5-rev1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\Projects\sympl\src\sympl\core\string_buffer.c > CMakeFiles\SymplLib.dir\src\sympl\core\string_buffer.c.i

CMakeFiles/SymplLib.dir/src/sympl/core/string_buffer.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SymplLib.dir/src/sympl/core/string_buffer.c.s"
	D:\MingGW\x86_64-6.2.0-posix-seh-rt_v5-rev1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S D:\Projects\sympl\src\sympl\core\string_buffer.c -o CMakeFiles\SymplLib.dir\src\sympl\core\string_buffer.c.s

CMakeFiles/SymplLib.dir/src/sympl/core/string_buffer.c.obj.requires:

.PHONY : CMakeFiles/SymplLib.dir/src/sympl/core/string_buffer.c.obj.requires

CMakeFiles/SymplLib.dir/src/sympl/core/string_buffer.c.obj.provides: CMakeFiles/SymplLib.dir/src/sympl/core/string_buffer.c.obj.requires
	$(MAKE) -f CMakeFiles\SymplLib.dir\build.make CMakeFiles/SymplLib.dir/src/sympl/core/string_buffer.c.obj.provides.build
.PHONY : CMakeFiles/SymplLib.dir/src/sympl/core/string_buffer.c.obj.provides

CMakeFiles/SymplLib.dir/src/sympl/core/string_buffer.c.obj.provides.build: CMakeFiles/SymplLib.dir/src/sympl/core/string_buffer.c.obj


CMakeFiles/SymplLib.dir/src/sympl/io/reader.c.obj: CMakeFiles/SymplLib.dir/flags.make
CMakeFiles/SymplLib.dir/src/sympl/io/reader.c.obj: ../src/sympl/io/reader.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\Projects\sympl\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/SymplLib.dir/src/sympl/io/reader.c.obj"
	D:\MingGW\x86_64-6.2.0-posix-seh-rt_v5-rev1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\SymplLib.dir\src\sympl\io\reader.c.obj   -c D:\Projects\sympl\src\sympl\io\reader.c

CMakeFiles/SymplLib.dir/src/sympl/io/reader.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SymplLib.dir/src/sympl/io/reader.c.i"
	D:\MingGW\x86_64-6.2.0-posix-seh-rt_v5-rev1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\Projects\sympl\src\sympl\io\reader.c > CMakeFiles\SymplLib.dir\src\sympl\io\reader.c.i

CMakeFiles/SymplLib.dir/src/sympl/io/reader.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SymplLib.dir/src/sympl/io/reader.c.s"
	D:\MingGW\x86_64-6.2.0-posix-seh-rt_v5-rev1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S D:\Projects\sympl\src\sympl\io\reader.c -o CMakeFiles\SymplLib.dir\src\sympl\io\reader.c.s

CMakeFiles/SymplLib.dir/src/sympl/io/reader.c.obj.requires:

.PHONY : CMakeFiles/SymplLib.dir/src/sympl/io/reader.c.obj.requires

CMakeFiles/SymplLib.dir/src/sympl/io/reader.c.obj.provides: CMakeFiles/SymplLib.dir/src/sympl/io/reader.c.obj.requires
	$(MAKE) -f CMakeFiles\SymplLib.dir\build.make CMakeFiles/SymplLib.dir/src/sympl/io/reader.c.obj.provides.build
.PHONY : CMakeFiles/SymplLib.dir/src/sympl/io/reader.c.obj.provides

CMakeFiles/SymplLib.dir/src/sympl/io/reader.c.obj.provides.build: CMakeFiles/SymplLib.dir/src/sympl/io/reader.c.obj


CMakeFiles/SymplLib.dir/src/sympl/io/writer.c.obj: CMakeFiles/SymplLib.dir/flags.make
CMakeFiles/SymplLib.dir/src/sympl/io/writer.c.obj: ../src/sympl/io/writer.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\Projects\sympl\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/SymplLib.dir/src/sympl/io/writer.c.obj"
	D:\MingGW\x86_64-6.2.0-posix-seh-rt_v5-rev1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\SymplLib.dir\src\sympl\io\writer.c.obj   -c D:\Projects\sympl\src\sympl\io\writer.c

CMakeFiles/SymplLib.dir/src/sympl/io/writer.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SymplLib.dir/src/sympl/io/writer.c.i"
	D:\MingGW\x86_64-6.2.0-posix-seh-rt_v5-rev1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\Projects\sympl\src\sympl\io\writer.c > CMakeFiles\SymplLib.dir\src\sympl\io\writer.c.i

CMakeFiles/SymplLib.dir/src/sympl/io/writer.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SymplLib.dir/src/sympl/io/writer.c.s"
	D:\MingGW\x86_64-6.2.0-posix-seh-rt_v5-rev1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S D:\Projects\sympl\src\sympl\io\writer.c -o CMakeFiles\SymplLib.dir\src\sympl\io\writer.c.s

CMakeFiles/SymplLib.dir/src/sympl/io/writer.c.obj.requires:

.PHONY : CMakeFiles/SymplLib.dir/src/sympl/io/writer.c.obj.requires

CMakeFiles/SymplLib.dir/src/sympl/io/writer.c.obj.provides: CMakeFiles/SymplLib.dir/src/sympl/io/writer.c.obj.requires
	$(MAKE) -f CMakeFiles\SymplLib.dir\build.make CMakeFiles/SymplLib.dir/src/sympl/io/writer.c.obj.provides.build
.PHONY : CMakeFiles/SymplLib.dir/src/sympl/io/writer.c.obj.provides

CMakeFiles/SymplLib.dir/src/sympl/io/writer.c.obj.provides.build: CMakeFiles/SymplLib.dir/src/sympl/io/writer.c.obj


CMakeFiles/SymplLib.dir/src/sympl/thirdparty/getoptutil.c.obj: CMakeFiles/SymplLib.dir/flags.make
CMakeFiles/SymplLib.dir/src/sympl/thirdparty/getoptutil.c.obj: ../src/sympl/thirdparty/getoptutil.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\Projects\sympl\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/SymplLib.dir/src/sympl/thirdparty/getoptutil.c.obj"
	D:\MingGW\x86_64-6.2.0-posix-seh-rt_v5-rev1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\SymplLib.dir\src\sympl\thirdparty\getoptutil.c.obj   -c D:\Projects\sympl\src\sympl\thirdparty\getoptutil.c

CMakeFiles/SymplLib.dir/src/sympl/thirdparty/getoptutil.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SymplLib.dir/src/sympl/thirdparty/getoptutil.c.i"
	D:\MingGW\x86_64-6.2.0-posix-seh-rt_v5-rev1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\Projects\sympl\src\sympl\thirdparty\getoptutil.c > CMakeFiles\SymplLib.dir\src\sympl\thirdparty\getoptutil.c.i

CMakeFiles/SymplLib.dir/src/sympl/thirdparty/getoptutil.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SymplLib.dir/src/sympl/thirdparty/getoptutil.c.s"
	D:\MingGW\x86_64-6.2.0-posix-seh-rt_v5-rev1\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S D:\Projects\sympl\src\sympl\thirdparty\getoptutil.c -o CMakeFiles\SymplLib.dir\src\sympl\thirdparty\getoptutil.c.s

CMakeFiles/SymplLib.dir/src/sympl/thirdparty/getoptutil.c.obj.requires:

.PHONY : CMakeFiles/SymplLib.dir/src/sympl/thirdparty/getoptutil.c.obj.requires

CMakeFiles/SymplLib.dir/src/sympl/thirdparty/getoptutil.c.obj.provides: CMakeFiles/SymplLib.dir/src/sympl/thirdparty/getoptutil.c.obj.requires
	$(MAKE) -f CMakeFiles\SymplLib.dir\build.make CMakeFiles/SymplLib.dir/src/sympl/thirdparty/getoptutil.c.obj.provides.build
.PHONY : CMakeFiles/SymplLib.dir/src/sympl/thirdparty/getoptutil.c.obj.provides

CMakeFiles/SymplLib.dir/src/sympl/thirdparty/getoptutil.c.obj.provides.build: CMakeFiles/SymplLib.dir/src/sympl/thirdparty/getoptutil.c.obj


# Object files for target SymplLib
SymplLib_OBJECTS = \
"CMakeFiles/SymplLib.dir/src/sympl/core/alloc.c.obj" \
"CMakeFiles/SymplLib.dir/src/sympl/core/resource.c.obj" \
"CMakeFiles/SymplLib.dir/src/sympl/core/string_buffer.c.obj" \
"CMakeFiles/SymplLib.dir/src/sympl/io/reader.c.obj" \
"CMakeFiles/SymplLib.dir/src/sympl/io/writer.c.obj" \
"CMakeFiles/SymplLib.dir/src/sympl/thirdparty/getoptutil.c.obj"

# External object files for target SymplLib
SymplLib_EXTERNAL_OBJECTS =

../builds/debug/libSymplLib.a: CMakeFiles/SymplLib.dir/src/sympl/core/alloc.c.obj
../builds/debug/libSymplLib.a: CMakeFiles/SymplLib.dir/src/sympl/core/resource.c.obj
../builds/debug/libSymplLib.a: CMakeFiles/SymplLib.dir/src/sympl/core/string_buffer.c.obj
../builds/debug/libSymplLib.a: CMakeFiles/SymplLib.dir/src/sympl/io/reader.c.obj
../builds/debug/libSymplLib.a: CMakeFiles/SymplLib.dir/src/sympl/io/writer.c.obj
../builds/debug/libSymplLib.a: CMakeFiles/SymplLib.dir/src/sympl/thirdparty/getoptutil.c.obj
../builds/debug/libSymplLib.a: CMakeFiles/SymplLib.dir/build.make
../builds/debug/libSymplLib.a: CMakeFiles/SymplLib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\Projects\sympl\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking C static library ..\builds\debug\libSymplLib.a"
	$(CMAKE_COMMAND) -P CMakeFiles\SymplLib.dir\cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\SymplLib.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/SymplLib.dir/build: ../builds/debug/libSymplLib.a

.PHONY : CMakeFiles/SymplLib.dir/build

CMakeFiles/SymplLib.dir/requires: CMakeFiles/SymplLib.dir/src/sympl/core/alloc.c.obj.requires
CMakeFiles/SymplLib.dir/requires: CMakeFiles/SymplLib.dir/src/sympl/core/resource.c.obj.requires
CMakeFiles/SymplLib.dir/requires: CMakeFiles/SymplLib.dir/src/sympl/core/string_buffer.c.obj.requires
CMakeFiles/SymplLib.dir/requires: CMakeFiles/SymplLib.dir/src/sympl/io/reader.c.obj.requires
CMakeFiles/SymplLib.dir/requires: CMakeFiles/SymplLib.dir/src/sympl/io/writer.c.obj.requires
CMakeFiles/SymplLib.dir/requires: CMakeFiles/SymplLib.dir/src/sympl/thirdparty/getoptutil.c.obj.requires

.PHONY : CMakeFiles/SymplLib.dir/requires

CMakeFiles/SymplLib.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\SymplLib.dir\cmake_clean.cmake
.PHONY : CMakeFiles/SymplLib.dir/clean

CMakeFiles/SymplLib.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\Projects\sympl D:\Projects\sympl D:\Projects\sympl\cmake-build-debug D:\Projects\sympl\cmake-build-debug D:\Projects\sympl\cmake-build-debug\CMakeFiles\SymplLib.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/SymplLib.dir/depend

