# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.9

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

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ethan/Desktop/os2/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ethan/Desktop/os2/build

# Include any dependencies generated for this target.
include CMakeFiles/os.bin.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/os.bin.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/os.bin.dir/flags.make

CMakeFiles/os.bin.dir/os/kernel/main.c.o: CMakeFiles/os.bin.dir/flags.make
CMakeFiles/os.bin.dir/os/kernel/main.c.o: /home/ethan/Desktop/os2/src/os/kernel/main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ethan/Desktop/os2/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/os.bin.dir/os/kernel/main.c.o"
	../cross-compiler/bin/i686-elf-gcc $(C_DEFINES) -D__FILENAME__=main.c $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/os.bin.dir/os/kernel/main.c.o   -c /home/ethan/Desktop/os2/src/os/kernel/main.c

CMakeFiles/os.bin.dir/os/kernel/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/os.bin.dir/os/kernel/main.c.i"
	../cross-compiler/bin/i686-elf-gcc $(C_DEFINES) -D__FILENAME__=main.c $(C_INCLUDES) $(C_FLAGS) -E /home/ethan/Desktop/os2/src/os/kernel/main.c > CMakeFiles/os.bin.dir/os/kernel/main.c.i

CMakeFiles/os.bin.dir/os/kernel/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/os.bin.dir/os/kernel/main.c.s"
	../cross-compiler/bin/i686-elf-gcc $(C_DEFINES) -D__FILENAME__=main.c $(C_INCLUDES) $(C_FLAGS) -S /home/ethan/Desktop/os2/src/os/kernel/main.c -o CMakeFiles/os.bin.dir/os/kernel/main.c.s

CMakeFiles/os.bin.dir/os/kernel/main.c.o.requires:

.PHONY : CMakeFiles/os.bin.dir/os/kernel/main.c.o.requires

CMakeFiles/os.bin.dir/os/kernel/main.c.o.provides: CMakeFiles/os.bin.dir/os/kernel/main.c.o.requires
	$(MAKE) -f CMakeFiles/os.bin.dir/build.make CMakeFiles/os.bin.dir/os/kernel/main.c.o.provides.build
.PHONY : CMakeFiles/os.bin.dir/os/kernel/main.c.o.provides

CMakeFiles/os.bin.dir/os/kernel/main.c.o.provides.build: CMakeFiles/os.bin.dir/os/kernel/main.c.o


# Object files for target os.bin
os_bin_OBJECTS = \
"CMakeFiles/os.bin.dir/os/kernel/main.c.o"

# External object files for target os.bin
os_bin_EXTERNAL_OBJECTS =

os.bin: CMakeFiles/os.bin.dir/os/kernel/main.c.o
os.bin: CMakeFiles/os.bin.dir/build.make
os.bin: CMakeFiles/os.bin.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ethan/Desktop/os2/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable os.bin"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/os.bin.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/os.bin.dir/build: os.bin

.PHONY : CMakeFiles/os.bin.dir/build

CMakeFiles/os.bin.dir/requires: CMakeFiles/os.bin.dir/os/kernel/main.c.o.requires

.PHONY : CMakeFiles/os.bin.dir/requires

CMakeFiles/os.bin.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/os.bin.dir/cmake_clean.cmake
.PHONY : CMakeFiles/os.bin.dir/clean

CMakeFiles/os.bin.dir/depend:
	cd /home/ethan/Desktop/os2/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ethan/Desktop/os2/src /home/ethan/Desktop/os2/src /home/ethan/Desktop/os2/build /home/ethan/Desktop/os2/build /home/ethan/Desktop/os2/build/CMakeFiles/os.bin.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/os.bin.dir/depend

