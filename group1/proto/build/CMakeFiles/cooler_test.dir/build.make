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

# Include any dependencies generated for this target.
include CMakeFiles/cooler_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/cooler_test.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/cooler_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/cooler_test.dir/flags.make

CMakeFiles/cooler_test.dir/tests/cooler_tests.cpp.o: CMakeFiles/cooler_test.dir/flags.make
CMakeFiles/cooler_test.dir/tests/cooler_tests.cpp.o: ../tests/cooler_tests.cpp
CMakeFiles/cooler_test.dir/tests/cooler_tests.cpp.o: CMakeFiles/cooler_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/juho/Desktop/conveyor/graduation_project/group1/proto/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/cooler_test.dir/tests/cooler_tests.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/cooler_test.dir/tests/cooler_tests.cpp.o -MF CMakeFiles/cooler_test.dir/tests/cooler_tests.cpp.o.d -o CMakeFiles/cooler_test.dir/tests/cooler_tests.cpp.o -c /home/juho/Desktop/conveyor/graduation_project/group1/proto/tests/cooler_tests.cpp

CMakeFiles/cooler_test.dir/tests/cooler_tests.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cooler_test.dir/tests/cooler_tests.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/juho/Desktop/conveyor/graduation_project/group1/proto/tests/cooler_tests.cpp > CMakeFiles/cooler_test.dir/tests/cooler_tests.cpp.i

CMakeFiles/cooler_test.dir/tests/cooler_tests.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cooler_test.dir/tests/cooler_tests.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/juho/Desktop/conveyor/graduation_project/group1/proto/tests/cooler_tests.cpp -o CMakeFiles/cooler_test.dir/tests/cooler_tests.cpp.s

# Object files for target cooler_test
cooler_test_OBJECTS = \
"CMakeFiles/cooler_test.dir/tests/cooler_tests.cpp.o"

# External object files for target cooler_test
cooler_test_EXTERNAL_OBJECTS =

tests/cooler_test: CMakeFiles/cooler_test.dir/tests/cooler_tests.cpp.o
tests/cooler_test: CMakeFiles/cooler_test.dir/build.make
tests/cooler_test: CMakeFiles/cooler_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/juho/Desktop/conveyor/graduation_project/group1/proto/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable tests/cooler_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cooler_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/cooler_test.dir/build: tests/cooler_test
.PHONY : CMakeFiles/cooler_test.dir/build

CMakeFiles/cooler_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/cooler_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/cooler_test.dir/clean

CMakeFiles/cooler_test.dir/depend:
	cd /home/juho/Desktop/conveyor/graduation_project/group1/proto/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/juho/Desktop/conveyor/graduation_project/group1/proto /home/juho/Desktop/conveyor/graduation_project/group1/proto /home/juho/Desktop/conveyor/graduation_project/group1/proto/build /home/juho/Desktop/conveyor/graduation_project/group1/proto/build /home/juho/Desktop/conveyor/graduation_project/group1/proto/build/CMakeFiles/cooler_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/cooler_test.dir/depend
