# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/juho/Desktop/conveyor/graduation_project/group1/proto

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/juho/Desktop/conveyor/graduation_project/group1/proto/build

# Utility rule file for test_conveyor.

# Include any custom commands dependencies for this target.
include CMakeFiles/test_conveyor.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/test_conveyor.dir/progress.make

CMakeFiles/test_conveyor: tests/conveyor_test
	tests/conveyor_test

test_conveyor: CMakeFiles/test_conveyor
test_conveyor: CMakeFiles/test_conveyor.dir/build.make
.PHONY : test_conveyor

# Rule to build all files generated by this target.
CMakeFiles/test_conveyor.dir/build: test_conveyor
.PHONY : CMakeFiles/test_conveyor.dir/build

CMakeFiles/test_conveyor.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_conveyor.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_conveyor.dir/clean

CMakeFiles/test_conveyor.dir/depend:
	cd /home/juho/Desktop/conveyor/graduation_project/group1/proto/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/juho/Desktop/conveyor/graduation_project/group1/proto /home/juho/Desktop/conveyor/graduation_project/group1/proto /home/juho/Desktop/conveyor/graduation_project/group1/proto/build /home/juho/Desktop/conveyor/graduation_project/group1/proto/build /home/juho/Desktop/conveyor/graduation_project/group1/proto/build/CMakeFiles/test_conveyor.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_conveyor.dir/depend

