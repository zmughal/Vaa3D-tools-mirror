# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
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

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build

# Include any dependencies generated for this target.
include Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/depend.make

# Include the progress variables for this target.
include Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/progress.make

# Include the compile flags for this target's objects.
include Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/flags.make

Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/ITKFastMarchingPlus.o: Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/flags.make
Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/ITKFastMarchingPlus.o: /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/LevelSets/ITKFastMarchingPlus.cxx
	$(CMAKE_COMMAND) -E cmake_progress_report /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/ITKFastMarchingPlus.o"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/LevelSets && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/ITKFastMarchingPlus.dir/ITKFastMarchingPlus.o -c /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/LevelSets/ITKFastMarchingPlus.cxx

Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/ITKFastMarchingPlus.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ITKFastMarchingPlus.dir/ITKFastMarchingPlus.i"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/LevelSets && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/LevelSets/ITKFastMarchingPlus.cxx > CMakeFiles/ITKFastMarchingPlus.dir/ITKFastMarchingPlus.i

Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/ITKFastMarchingPlus.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ITKFastMarchingPlus.dir/ITKFastMarchingPlus.s"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/LevelSets && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/LevelSets/ITKFastMarchingPlus.cxx -o CMakeFiles/ITKFastMarchingPlus.dir/ITKFastMarchingPlus.s

Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/ITKFastMarchingPlus.o.requires:
.PHONY : Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/ITKFastMarchingPlus.o.requires

Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/ITKFastMarchingPlus.o.provides: Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/ITKFastMarchingPlus.o.requires
	$(MAKE) -f Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/build.make Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/ITKFastMarchingPlus.o.provides.build
.PHONY : Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/ITKFastMarchingPlus.o.provides

Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/ITKFastMarchingPlus.o.provides.build: Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/ITKFastMarchingPlus.o
.PHONY : Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/ITKFastMarchingPlus.o.provides.build

Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/moc_ITKFastMarchingPlus.o: Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/flags.make
Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/moc_ITKFastMarchingPlus.o: Source/LevelSets/moc_ITKFastMarchingPlus.cxx
	$(CMAKE_COMMAND) -E cmake_progress_report /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/moc_ITKFastMarchingPlus.o"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/LevelSets && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/ITKFastMarchingPlus.dir/moc_ITKFastMarchingPlus.o -c /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/LevelSets/moc_ITKFastMarchingPlus.cxx

Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/moc_ITKFastMarchingPlus.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ITKFastMarchingPlus.dir/moc_ITKFastMarchingPlus.i"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/LevelSets && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/LevelSets/moc_ITKFastMarchingPlus.cxx > CMakeFiles/ITKFastMarchingPlus.dir/moc_ITKFastMarchingPlus.i

Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/moc_ITKFastMarchingPlus.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ITKFastMarchingPlus.dir/moc_ITKFastMarchingPlus.s"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/LevelSets && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/LevelSets/moc_ITKFastMarchingPlus.cxx -o CMakeFiles/ITKFastMarchingPlus.dir/moc_ITKFastMarchingPlus.s

Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/moc_ITKFastMarchingPlus.o.requires:
.PHONY : Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/moc_ITKFastMarchingPlus.o.requires

Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/moc_ITKFastMarchingPlus.o.provides: Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/moc_ITKFastMarchingPlus.o.requires
	$(MAKE) -f Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/build.make Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/moc_ITKFastMarchingPlus.o.provides.build
.PHONY : Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/moc_ITKFastMarchingPlus.o.provides

Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/moc_ITKFastMarchingPlus.o.provides.build: Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/moc_ITKFastMarchingPlus.o
.PHONY : Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/moc_ITKFastMarchingPlus.o.provides.build

Source/LevelSets/moc_ITKFastMarchingPlus.cxx: /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/LevelSets/ITKFastMarchingPlus.h
	$(CMAKE_COMMAND) -E cmake_progress_report /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating moc_ITKFastMarchingPlus.cxx"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/LevelSets && /usr/bin/moc-qt4 -I/home/liyun/V3D/v3d_main/basic_c_fun/../common_lib/include -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Review/Statistics -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Review -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/gdcm/src -I/home/liyun/ITK/ITK-bin/Utilities/gdcm -I/home/liyun/ITK/ITK-bin/Utilities/vxl/core -I/home/liyun/ITK/ITK-bin/Utilities/vxl/vcl -I/home/liyun/ITK/ITK-bin/Utilities/vxl/v3p/netlib -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/vxl/core -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/vxl/vcl -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/vxl/v3p/netlib -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities -I/home/liyun/ITK/ITK-bin/Utilities -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/itkExtHdrs -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/nifti/znzlib -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/nifti/niftilib -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/expat -I/home/liyun/ITK/ITK-bin/Utilities/expat -I/home/liyun/ITK/ITK-bin/Utilities/DICOMParser -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/DICOMParser -I/home/liyun/ITK/ITK-bin/Utilities/NrrdIO -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/NrrdIO -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/MetaIO -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/SpatialObject -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Numerics/NeuralNetworks -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Numerics/FEM -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/IO -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Numerics -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Common -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/BasicFilters -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Algorithms -I/home/liyun/ITK/ITK-bin -I/usr/include/qt4 -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtCore -I/home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Common -I/home/liyun/V3D/v3d_main/basic_c_fun -I/home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins -DQT_GUI_LIB -DQT_CORE_LIB -o /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/LevelSets/moc_ITKFastMarchingPlus.cxx /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/LevelSets/ITKFastMarchingPlus.h

# Object files for target ITKFastMarchingPlus
ITKFastMarchingPlus_OBJECTS = \
"CMakeFiles/ITKFastMarchingPlus.dir/ITKFastMarchingPlus.o" \
"CMakeFiles/ITKFastMarchingPlus.dir/moc_ITKFastMarchingPlus.o"

# External object files for target ITKFastMarchingPlus
ITKFastMarchingPlus_EXTERNAL_OBJECTS =

bin/libITKFastMarchingPlus.so: Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/ITKFastMarchingPlus.o
bin/libITKFastMarchingPlus.so: Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/moc_ITKFastMarchingPlus.o
bin/libITKFastMarchingPlus.so: bin/libV3DInterface.a
bin/libITKFastMarchingPlus.so: bin/libV3DITKCommon.a
bin/libITKFastMarchingPlus.so: /usr/lib/libQtGui.so
bin/libITKFastMarchingPlus.so: /usr/lib/libQtCore.so
bin/libITKFastMarchingPlus.so: Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/build.make
bin/libITKFastMarchingPlus.so: Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX shared library ../../bin/libITKFastMarchingPlus.so"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/LevelSets && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ITKFastMarchingPlus.dir/link.txt --verbose=$(VERBOSE)
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/LevelSets && /usr/bin/cmake -E copy /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/bin/libITKFastMarchingPlus.so /home/liyun/V3D/v3d/plugins/ITK//LevelSets/ITKFastMarchingPlus/ITKFastMarchingPlus.so

# Rule to build all files generated by this target.
Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/build: bin/libITKFastMarchingPlus.so
.PHONY : Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/build

Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/requires: Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/ITKFastMarchingPlus.o.requires
Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/requires: Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/moc_ITKFastMarchingPlus.o.requires
.PHONY : Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/requires

Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/clean:
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/LevelSets && $(CMAKE_COMMAND) -P CMakeFiles/ITKFastMarchingPlus.dir/cmake_clean.cmake
.PHONY : Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/clean

Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/depend: Source/LevelSets/moc_ITKFastMarchingPlus.cxx
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/LevelSets /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/LevelSets /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Source/LevelSets/CMakeFiles/ITKFastMarchingPlus.dir/depend

