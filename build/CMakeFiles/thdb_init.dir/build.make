# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_SOURCE_DIR = /home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/build

# Include any dependencies generated for this target.
include CMakeFiles/thdb_init.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/thdb_init.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/thdb_init.dir/flags.make

CMakeFiles/thdb_init.dir/executable/init.cc.o: CMakeFiles/thdb_init.dir/flags.make
CMakeFiles/thdb_init.dir/executable/init.cc.o: ../executable/init.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/thdb_init.dir/executable/init.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/thdb_init.dir/executable/init.cc.o -c /home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/executable/init.cc

CMakeFiles/thdb_init.dir/executable/init.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/thdb_init.dir/executable/init.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/executable/init.cc > CMakeFiles/thdb_init.dir/executable/init.cc.i

CMakeFiles/thdb_init.dir/executable/init.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/thdb_init.dir/executable/init.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/executable/init.cc -o CMakeFiles/thdb_init.dir/executable/init.cc.s

CMakeFiles/thdb_init.dir/executable/init.cc.o.requires:

.PHONY : CMakeFiles/thdb_init.dir/executable/init.cc.o.requires

CMakeFiles/thdb_init.dir/executable/init.cc.o.provides: CMakeFiles/thdb_init.dir/executable/init.cc.o.requires
	$(MAKE) -f CMakeFiles/thdb_init.dir/build.make CMakeFiles/thdb_init.dir/executable/init.cc.o.provides.build
.PHONY : CMakeFiles/thdb_init.dir/executable/init.cc.o.provides

CMakeFiles/thdb_init.dir/executable/init.cc.o.provides.build: CMakeFiles/thdb_init.dir/executable/init.cc.o


# Object files for target thdb_init
thdb_init_OBJECTS = \
"CMakeFiles/thdb_init.dir/executable/init.cc.o"

# External object files for target thdb_init
thdb_init_EXTERNAL_OBJECTS =

bin/thdb_init: CMakeFiles/thdb_init.dir/executable/init.cc.o
bin/thdb_init: CMakeFiles/thdb_init.dir/build.make
bin/thdb_init: lib/libthdb_shared.so
bin/thdb_init: lib/libantlr4-runtime.so
bin/thdb_init: CMakeFiles/thdb_init.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable bin/thdb_init"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/thdb_init.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/thdb_init.dir/build: bin/thdb_init

.PHONY : CMakeFiles/thdb_init.dir/build

CMakeFiles/thdb_init.dir/requires: CMakeFiles/thdb_init.dir/executable/init.cc.o.requires

.PHONY : CMakeFiles/thdb_init.dir/requires

CMakeFiles/thdb_init.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/thdb_init.dir/cmake_clean.cmake
.PHONY : CMakeFiles/thdb_init.dir/clean

CMakeFiles/thdb_init.dir/depend:
	cd /home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067 /home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067 /home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/build /home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/build /home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/build/CMakeFiles/thdb_init.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/thdb_init.dir/depend
