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
include CMakeFiles/sylar.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/sylar.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/sylar.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/sylar.dir/flags.make

CMakeFiles/sylar.dir/src/util.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/src/util.cc.o: ../src/util.cc
CMakeFiles/sylar.dir/src/util.cc.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/huang/workspace/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/sylar.dir/src/util.cc.o"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"src/util.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/src/util.cc.o -MF CMakeFiles/sylar.dir/src/util.cc.o.d -o CMakeFiles/sylar.dir/src/util.cc.o -c /home/huang/workspace/sylar_learn/src/util.cc

CMakeFiles/sylar.dir/src/util.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sylar.dir/src/util.cc.i"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"src/util.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/huang/workspace/sylar_learn/src/util.cc > CMakeFiles/sylar.dir/src/util.cc.i

CMakeFiles/sylar.dir/src/util.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/src/util.cc.s"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"src/util.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/huang/workspace/sylar_learn/src/util.cc -o CMakeFiles/sylar.dir/src/util.cc.s

CMakeFiles/sylar.dir/src/fiber.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/src/fiber.cc.o: ../src/fiber.cc
CMakeFiles/sylar.dir/src/fiber.cc.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/huang/workspace/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/sylar.dir/src/fiber.cc.o"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"src/fiber.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/src/fiber.cc.o -MF CMakeFiles/sylar.dir/src/fiber.cc.o.d -o CMakeFiles/sylar.dir/src/fiber.cc.o -c /home/huang/workspace/sylar_learn/src/fiber.cc

CMakeFiles/sylar.dir/src/fiber.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sylar.dir/src/fiber.cc.i"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"src/fiber.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/huang/workspace/sylar_learn/src/fiber.cc > CMakeFiles/sylar.dir/src/fiber.cc.i

CMakeFiles/sylar.dir/src/fiber.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/src/fiber.cc.s"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"src/fiber.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/huang/workspace/sylar_learn/src/fiber.cc -o CMakeFiles/sylar.dir/src/fiber.cc.s

CMakeFiles/sylar.dir/src/log.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/src/log.cc.o: ../src/log.cc
CMakeFiles/sylar.dir/src/log.cc.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/huang/workspace/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/sylar.dir/src/log.cc.o"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"src/log.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/src/log.cc.o -MF CMakeFiles/sylar.dir/src/log.cc.o.d -o CMakeFiles/sylar.dir/src/log.cc.o -c /home/huang/workspace/sylar_learn/src/log.cc

CMakeFiles/sylar.dir/src/log.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sylar.dir/src/log.cc.i"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"src/log.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/huang/workspace/sylar_learn/src/log.cc > CMakeFiles/sylar.dir/src/log.cc.i

CMakeFiles/sylar.dir/src/log.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/src/log.cc.s"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"src/log.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/huang/workspace/sylar_learn/src/log.cc -o CMakeFiles/sylar.dir/src/log.cc.s

CMakeFiles/sylar.dir/src/config.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/src/config.cc.o: ../src/config.cc
CMakeFiles/sylar.dir/src/config.cc.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/huang/workspace/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/sylar.dir/src/config.cc.o"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"src/config.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/src/config.cc.o -MF CMakeFiles/sylar.dir/src/config.cc.o.d -o CMakeFiles/sylar.dir/src/config.cc.o -c /home/huang/workspace/sylar_learn/src/config.cc

CMakeFiles/sylar.dir/src/config.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sylar.dir/src/config.cc.i"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"src/config.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/huang/workspace/sylar_learn/src/config.cc > CMakeFiles/sylar.dir/src/config.cc.i

CMakeFiles/sylar.dir/src/config.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/src/config.cc.s"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"src/config.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/huang/workspace/sylar_learn/src/config.cc -o CMakeFiles/sylar.dir/src/config.cc.s

CMakeFiles/sylar.dir/src/thread.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/src/thread.cc.o: ../src/thread.cc
CMakeFiles/sylar.dir/src/thread.cc.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/huang/workspace/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/sylar.dir/src/thread.cc.o"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"src/thread.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/src/thread.cc.o -MF CMakeFiles/sylar.dir/src/thread.cc.o.d -o CMakeFiles/sylar.dir/src/thread.cc.o -c /home/huang/workspace/sylar_learn/src/thread.cc

CMakeFiles/sylar.dir/src/thread.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sylar.dir/src/thread.cc.i"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"src/thread.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/huang/workspace/sylar_learn/src/thread.cc > CMakeFiles/sylar.dir/src/thread.cc.i

CMakeFiles/sylar.dir/src/thread.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/src/thread.cc.s"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"src/thread.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/huang/workspace/sylar_learn/src/thread.cc -o CMakeFiles/sylar.dir/src/thread.cc.s

CMakeFiles/sylar.dir/src/mutex.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/src/mutex.cc.o: ../src/mutex.cc
CMakeFiles/sylar.dir/src/mutex.cc.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/huang/workspace/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/sylar.dir/src/mutex.cc.o"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"src/mutex.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/src/mutex.cc.o -MF CMakeFiles/sylar.dir/src/mutex.cc.o.d -o CMakeFiles/sylar.dir/src/mutex.cc.o -c /home/huang/workspace/sylar_learn/src/mutex.cc

CMakeFiles/sylar.dir/src/mutex.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sylar.dir/src/mutex.cc.i"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"src/mutex.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/huang/workspace/sylar_learn/src/mutex.cc > CMakeFiles/sylar.dir/src/mutex.cc.i

CMakeFiles/sylar.dir/src/mutex.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/src/mutex.cc.s"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"src/mutex.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/huang/workspace/sylar_learn/src/mutex.cc -o CMakeFiles/sylar.dir/src/mutex.cc.s

CMakeFiles/sylar.dir/src/scheduler.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/src/scheduler.cc.o: ../src/scheduler.cc
CMakeFiles/sylar.dir/src/scheduler.cc.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/huang/workspace/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/sylar.dir/src/scheduler.cc.o"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"src/scheduler.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/src/scheduler.cc.o -MF CMakeFiles/sylar.dir/src/scheduler.cc.o.d -o CMakeFiles/sylar.dir/src/scheduler.cc.o -c /home/huang/workspace/sylar_learn/src/scheduler.cc

CMakeFiles/sylar.dir/src/scheduler.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sylar.dir/src/scheduler.cc.i"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"src/scheduler.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/huang/workspace/sylar_learn/src/scheduler.cc > CMakeFiles/sylar.dir/src/scheduler.cc.i

CMakeFiles/sylar.dir/src/scheduler.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/src/scheduler.cc.s"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"src/scheduler.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/huang/workspace/sylar_learn/src/scheduler.cc -o CMakeFiles/sylar.dir/src/scheduler.cc.s

CMakeFiles/sylar.dir/src/iomanager.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/src/iomanager.cc.o: ../src/iomanager.cc
CMakeFiles/sylar.dir/src/iomanager.cc.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/huang/workspace/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/sylar.dir/src/iomanager.cc.o"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"src/iomanager.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/src/iomanager.cc.o -MF CMakeFiles/sylar.dir/src/iomanager.cc.o.d -o CMakeFiles/sylar.dir/src/iomanager.cc.o -c /home/huang/workspace/sylar_learn/src/iomanager.cc

CMakeFiles/sylar.dir/src/iomanager.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sylar.dir/src/iomanager.cc.i"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"src/iomanager.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/huang/workspace/sylar_learn/src/iomanager.cc > CMakeFiles/sylar.dir/src/iomanager.cc.i

CMakeFiles/sylar.dir/src/iomanager.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/src/iomanager.cc.s"
	/usr/bin/x86_64-redhat-linux-g++ $(CXX_DEFINES) -D__FILE__=\"src/iomanager.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/huang/workspace/sylar_learn/src/iomanager.cc -o CMakeFiles/sylar.dir/src/iomanager.cc.s

# Object files for target sylar
sylar_OBJECTS = \
"CMakeFiles/sylar.dir/src/util.cc.o" \
"CMakeFiles/sylar.dir/src/fiber.cc.o" \
"CMakeFiles/sylar.dir/src/log.cc.o" \
"CMakeFiles/sylar.dir/src/config.cc.o" \
"CMakeFiles/sylar.dir/src/thread.cc.o" \
"CMakeFiles/sylar.dir/src/mutex.cc.o" \
"CMakeFiles/sylar.dir/src/scheduler.cc.o" \
"CMakeFiles/sylar.dir/src/iomanager.cc.o"

# External object files for target sylar
sylar_EXTERNAL_OBJECTS =

libsylar.so: CMakeFiles/sylar.dir/src/util.cc.o
libsylar.so: CMakeFiles/sylar.dir/src/fiber.cc.o
libsylar.so: CMakeFiles/sylar.dir/src/log.cc.o
libsylar.so: CMakeFiles/sylar.dir/src/config.cc.o
libsylar.so: CMakeFiles/sylar.dir/src/thread.cc.o
libsylar.so: CMakeFiles/sylar.dir/src/mutex.cc.o
libsylar.so: CMakeFiles/sylar.dir/src/scheduler.cc.o
libsylar.so: CMakeFiles/sylar.dir/src/iomanager.cc.o
libsylar.so: CMakeFiles/sylar.dir/build.make
libsylar.so: CMakeFiles/sylar.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/huang/workspace/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Linking CXX shared library libsylar.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/sylar.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/sylar.dir/build: libsylar.so
.PHONY : CMakeFiles/sylar.dir/build

CMakeFiles/sylar.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/sylar.dir/cmake_clean.cmake
.PHONY : CMakeFiles/sylar.dir/clean

CMakeFiles/sylar.dir/depend:
	cd /home/huang/workspace/sylar_learn/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/huang/workspace/sylar_learn /home/huang/workspace/sylar_learn /home/huang/workspace/sylar_learn/build /home/huang/workspace/sylar_learn/build /home/huang/workspace/sylar_learn/build/CMakeFiles/sylar.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/sylar.dir/depend

