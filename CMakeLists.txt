# Minimal CMake build file
# Builds:
# - static library from lib directory
# - commandline executable
# - test suite
# - Qt GUI

# To build with CMake:
# - install CMake 2.6 or later
# - $ cmake .
# - $ make

cmake_minimum_required (VERSION 2.6)

PROJECT(CPPCHECK)

# Building lib as static library is disabled due to bug
# #1299 CMake: The CheckClass is not used
# https://sourceforge.net/apps/trac/cppcheck/ticket/1299
# Instead lib code is included directly into cli and gui targets
# ADD_SUBDIRECTORY(lib)

ADD_SUBDIRECTORY(cli)

# Exclude tests from command line targets but include them to VS IDE targets.
# There is 'make check' -target for compiling and running tests from
# command line.
IF (MSVC_IDE)
  ADD_SUBDIRECTORY(test)
ELSE (MSVC_IDE)
  ADD_SUBDIRECTORY(test EXCLUDE_FROM_ALL)
ENDIF (MSVC_IDE)

ADD_SUBDIRECTORY(gui)
ADD_SUBDIRECTORY(man)
