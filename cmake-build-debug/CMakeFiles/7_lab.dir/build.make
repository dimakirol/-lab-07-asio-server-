# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

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
CMAKE_COMMAND = /home/kirill/Documents/Clion/clion-2019.3.4/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/kirill/Documents/Clion/clion-2019.3.4/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/kirill/Documents/4_semestr/-lab-07-asio-server-

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kirill/Documents/4_semestr/-lab-07-asio-server-/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/7_lab.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/7_lab.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/7_lab.dir/flags.make

CMakeFiles/7_lab.dir/sources/source.cpp.o: CMakeFiles/7_lab.dir/flags.make
CMakeFiles/7_lab.dir/sources/source.cpp.o: ../sources/source.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kirill/Documents/4_semestr/-lab-07-asio-server-/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/7_lab.dir/sources/source.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/7_lab.dir/sources/source.cpp.o -c /home/kirill/Documents/4_semestr/-lab-07-asio-server-/sources/source.cpp

CMakeFiles/7_lab.dir/sources/source.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/7_lab.dir/sources/source.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kirill/Documents/4_semestr/-lab-07-asio-server-/sources/source.cpp > CMakeFiles/7_lab.dir/sources/source.cpp.i

CMakeFiles/7_lab.dir/sources/source.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/7_lab.dir/sources/source.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kirill/Documents/4_semestr/-lab-07-asio-server-/sources/source.cpp -o CMakeFiles/7_lab.dir/sources/source.cpp.s

# Object files for target 7_lab
7_lab_OBJECTS = \
"CMakeFiles/7_lab.dir/sources/source.cpp.o"

# External object files for target 7_lab
7_lab_EXTERNAL_OBJECTS =

7_lab: CMakeFiles/7_lab.dir/sources/source.cpp.o
7_lab: CMakeFiles/7_lab.dir/build.make
7_lab: /home/kirill/Documents/boost/installed/lib/libboost_thread.so
7_lab: /home/kirill/Documents/boost/installed/lib/libboost_log.so
7_lab: /home/kirill/Documents/boost/installed/lib/libboost_log_setup.so
7_lab: /home/kirill/Documents/boost/installed/lib/libboost_chrono.so
7_lab: /home/kirill/Documents/boost/installed/lib/libboost_date_time.so
7_lab: /home/kirill/Documents/boost/installed/lib/libboost_atomic.so
7_lab: /home/kirill/Documents/boost/installed/lib/libboost_filesystem.so
7_lab: /home/kirill/Documents/boost/installed/lib/libboost_regex.so
7_lab: CMakeFiles/7_lab.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/kirill/Documents/4_semestr/-lab-07-asio-server-/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable 7_lab"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/7_lab.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/7_lab.dir/build: 7_lab

.PHONY : CMakeFiles/7_lab.dir/build

CMakeFiles/7_lab.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/7_lab.dir/cmake_clean.cmake
.PHONY : CMakeFiles/7_lab.dir/clean

CMakeFiles/7_lab.dir/depend:
	cd /home/kirill/Documents/4_semestr/-lab-07-asio-server-/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kirill/Documents/4_semestr/-lab-07-asio-server- /home/kirill/Documents/4_semestr/-lab-07-asio-server- /home/kirill/Documents/4_semestr/-lab-07-asio-server-/cmake-build-debug /home/kirill/Documents/4_semestr/-lab-07-asio-server-/cmake-build-debug /home/kirill/Documents/4_semestr/-lab-07-asio-server-/cmake-build-debug/CMakeFiles/7_lab.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/7_lab.dir/depend

