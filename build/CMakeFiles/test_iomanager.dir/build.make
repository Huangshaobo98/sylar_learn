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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/huang/workspace/sylar_learn

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/huang/workspace/sylar_learn/build

# Include any dependencies generated for this target.
include CMakeFiles/test_iomanager.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/test_iomanager.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/test_iomanager.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_iomanager.dir/flags.make

CMakeFiles/test_iomanager.dir/tests/test_iomanager.cc.o: CMakeFiles/test_iomanager.dir/flags.make
CMakeFiles/test_iomanager.dir/tests/test_iomanager.cc.o: ../tests/test_iomanager.cc
CMakeFiles/test_iomanager.dir/tests/test_iomanager.cc.o: CMakeFiles/test_iomanager.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/huang/workspace/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/test_iomanager.dir/tests/test_iomanager.cc.o"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"tests/test_iomanager.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/test_iomanager.dir/tests/test_iomanager.cc.o -MF CMakeFiles/test_iomanager.dir/tests/test_iomanager.cc.o.d -o CMakeFiles/test_iomanager.dir/tests/test_iomanager.cc.o -c /home/huang/workspace/sylar_learn/tests/test_iomanager.cc

CMakeFiles/test_iomanager.dir/tests/test_iomanager.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_iomanager.dir/tests/test_iomanager.cc.i"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"tests/test_iomanager.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/huang/workspace/sylar_learn/tests/test_iomanager.cc > CMakeFiles/test_iomanager.dir/tests/test_iomanager.cc.i

CMakeFiles/test_iomanager.dir/tests/test_iomanager.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_iomanager.dir/tests/test_iomanager.cc.s"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"tests/test_iomanager.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/huang/workspace/sylar_learn/tests/test_iomanager.cc -o CMakeFiles/test_iomanager.dir/tests/test_iomanager.cc.s

# Object files for target test_iomanager
test_iomanager_OBJECTS = \
"CMakeFiles/test_iomanager.dir/tests/test_iomanager.cc.o"

# External object files for target test_iomanager
test_iomanager_EXTERNAL_OBJECTS =

test_iomanager: CMakeFiles/test_iomanager.dir/tests/test_iomanager.cc.o
test_iomanager: CMakeFiles/test_iomanager.dir/build.make
test_iomanager: libsylar.so
test_iomanager: CMakeFiles/test_iomanager.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/huang/workspace/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable test_iomanager"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_iomanager.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_iomanager.dir/build: test_iomanager
.PHONY : CMakeFiles/test_iomanager.dir/build

CMakeFiles/test_iomanager.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_iomanager.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_iomanager.dir/clean

CMakeFiles/test_iomanager.dir/depend:
	cd /home/huang/workspace/sylar_learn/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/huang/workspace/sylar_learn /home/huang/workspace/sylar_learn /home/huang/workspace/sylar_learn/build /home/huang/workspace/sylar_learn/build /home/huang/workspace/sylar_learn/build/CMakeFiles/test_iomanager.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_iomanager.dir/depend

