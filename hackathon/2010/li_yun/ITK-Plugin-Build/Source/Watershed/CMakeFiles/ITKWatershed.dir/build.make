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
include Source/Watershed/CMakeFiles/ITKWatershed.dir/depend.make

# Include the progress variables for this target.
include Source/Watershed/CMakeFiles/ITKWatershed.dir/progress.make

# Include the compile flags for this target's objects.
include Source/Watershed/CMakeFiles/ITKWatershed.dir/flags.make

Source/Watershed/CMakeFiles/ITKWatershed.dir/ITKWatershed.o: Source/Watershed/CMakeFiles/ITKWatershed.dir/flags.make
Source/Watershed/CMakeFiles/ITKWatershed.dir/ITKWatershed.o: /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Watershed/ITKWatershed.cxx
	$(CMAKE_COMMAND) -E cmake_progress_report /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object Source/Watershed/CMakeFiles/ITKWatershed.dir/ITKWatershed.o"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Watershed && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/ITKWatershed.dir/ITKWatershed.o -c /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Watershed/ITKWatershed.cxx

Source/Watershed/CMakeFiles/ITKWatershed.dir/ITKWatershed.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ITKWatershed.dir/ITKWatershed.i"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Watershed && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Watershed/ITKWatershed.cxx > CMakeFiles/ITKWatershed.dir/ITKWatershed.i

Source/Watershed/CMakeFiles/ITKWatershed.dir/ITKWatershed.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ITKWatershed.dir/ITKWatershed.s"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Watershed && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Watershed/ITKWatershed.cxx -o CMakeFiles/ITKWatershed.dir/ITKWatershed.s

Source/Watershed/CMakeFiles/ITKWatershed.dir/ITKWatershed.o.requires:
.PHONY : Source/Watershed/CMakeFiles/ITKWatershed.dir/ITKWatershed.o.requires

Source/Watershed/CMakeFiles/ITKWatershed.dir/ITKWatershed.o.provides: Source/Watershed/CMakeFiles/ITKWatershed.dir/ITKWatershed.o.requires
	$(MAKE) -f Source/Watershed/CMakeFiles/ITKWatershed.dir/build.make Source/Watershed/CMakeFiles/ITKWatershed.dir/ITKWatershed.o.provides.build
.PHONY : Source/Watershed/CMakeFiles/ITKWatershed.dir/ITKWatershed.o.provides

Source/Watershed/CMakeFiles/ITKWatershed.dir/ITKWatershed.o.provides.build: Source/Watershed/CMakeFiles/ITKWatershed.dir/ITKWatershed.o
.PHONY : Source/Watershed/CMakeFiles/ITKWatershed.dir/ITKWatershed.o.provides.build

Source/Watershed/CMakeFiles/ITKWatershed.dir/moc_ITKWatershed.o: Source/Watershed/CMakeFiles/ITKWatershed.dir/flags.make
Source/Watershed/CMakeFiles/ITKWatershed.dir/moc_ITKWatershed.o: Source/Watershed/moc_ITKWatershed.cxx
	$(CMAKE_COMMAND) -E cmake_progress_report /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object Source/Watershed/CMakeFiles/ITKWatershed.dir/moc_ITKWatershed.o"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Watershed && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/ITKWatershed.dir/moc_ITKWatershed.o -c /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Watershed/moc_ITKWatershed.cxx

Source/Watershed/CMakeFiles/ITKWatershed.dir/moc_ITKWatershed.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ITKWatershed.dir/moc_ITKWatershed.i"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Watershed && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Watershed/moc_ITKWatershed.cxx > CMakeFiles/ITKWatershed.dir/moc_ITKWatershed.i

Source/Watershed/CMakeFiles/ITKWatershed.dir/moc_ITKWatershed.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ITKWatershed.dir/moc_ITKWatershed.s"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Watershed && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Watershed/moc_ITKWatershed.cxx -o CMakeFiles/ITKWatershed.dir/moc_ITKWatershed.s

Source/Watershed/CMakeFiles/ITKWatershed.dir/moc_ITKWatershed.o.requires:
.PHONY : Source/Watershed/CMakeFiles/ITKWatershed.dir/moc_ITKWatershed.o.requires

Source/Watershed/CMakeFiles/ITKWatershed.dir/moc_ITKWatershed.o.provides: Source/Watershed/CMakeFiles/ITKWatershed.dir/moc_ITKWatershed.o.requires
	$(MAKE) -f Source/Watershed/CMakeFiles/ITKWatershed.dir/build.make Source/Watershed/CMakeFiles/ITKWatershed.dir/moc_ITKWatershed.o.provides.build
.PHONY : Source/Watershed/CMakeFiles/ITKWatershed.dir/moc_ITKWatershed.o.provides

Source/Watershed/CMakeFiles/ITKWatershed.dir/moc_ITKWatershed.o.provides.build: Source/Watershed/CMakeFiles/ITKWatershed.dir/moc_ITKWatershed.o
.PHONY : Source/Watershed/CMakeFiles/ITKWatershed.dir/moc_ITKWatershed.o.provides.build

Source/Watershed/moc_ITKWatershed.cxx: /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Watershed/ITKWatershed.h
	$(CMAKE_COMMAND) -E cmake_progress_report /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating moc_ITKWatershed.cxx"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Watershed && /usr/bin/moc-qt4 -I/home/liyun/V3D/v3d_main/basic_c_fun/../common_lib/include -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Review/Statistics -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Review -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/gdcm/src -I/home/liyun/ITK/ITK-bin/Utilities/gdcm -I/home/liyun/ITK/ITK-bin/Utilities/vxl/core -I/home/liyun/ITK/ITK-bin/Utilities/vxl/vcl -I/home/liyun/ITK/ITK-bin/Utilities/vxl/v3p/netlib -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/vxl/core -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/vxl/vcl -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/vxl/v3p/netlib -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities -I/home/liyun/ITK/ITK-bin/Utilities -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/itkExtHdrs -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/nifti/znzlib -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/nifti/niftilib -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/expat -I/home/liyun/ITK/ITK-bin/Utilities/expat -I/home/liyun/ITK/ITK-bin/Utilities/DICOMParser -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/DICOMParser -I/home/liyun/ITK/ITK-bin/Utilities/NrrdIO -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/NrrdIO -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/MetaIO -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/SpatialObject -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Numerics/NeuralNetworks -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Numerics/FEM -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/IO -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Numerics -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Common -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/BasicFilters -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Algorithms -I/home/liyun/ITK/ITK-bin -I/usr/include/qt4 -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtCore -I/home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Common -I/home/liyun/V3D/v3d_main/basic_c_fun -I/home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins -DQT_GUI_LIB -DQT_CORE_LIB -o /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Watershed/moc_ITKWatershed.cxx /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Watershed/ITKWatershed.h

# Object files for target ITKWatershed
ITKWatershed_OBJECTS = \
"CMakeFiles/ITKWatershed.dir/ITKWatershed.o" \
"CMakeFiles/ITKWatershed.dir/moc_ITKWatershed.o"

# External object files for target ITKWatershed
ITKWatershed_EXTERNAL_OBJECTS =

bin/libITKWatershed.so: Source/Watershed/CMakeFiles/ITKWatershed.dir/ITKWatershed.o
bin/libITKWatershed.so: Source/Watershed/CMakeFiles/ITKWatershed.dir/moc_ITKWatershed.o
bin/libITKWatershed.so: bin/libV3DInterface.a
bin/libITKWatershed.so: bin/libV3DITKCommon.a
bin/libITKWatershed.so: /usr/lib/libQtGui.so
bin/libITKWatershed.so: /usr/lib/libQtCore.so
bin/libITKWatershed.so: Source/Watershed/CMakeFiles/ITKWatershed.dir/build.make
bin/libITKWatershed.so: Source/Watershed/CMakeFiles/ITKWatershed.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX shared library ../../bin/libITKWatershed.so"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Watershed && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ITKWatershed.dir/link.txt --verbose=$(VERBOSE)
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Watershed && /usr/bin/cmake -E copy /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/bin/libITKWatershed.so /home/liyun/V3D/v3d/plugins/ITK//Watershed/ITKWatershed/ITKWatershed.so

# Rule to build all files generated by this target.
Source/Watershed/CMakeFiles/ITKWatershed.dir/build: bin/libITKWatershed.so
.PHONY : Source/Watershed/CMakeFiles/ITKWatershed.dir/build

Source/Watershed/CMakeFiles/ITKWatershed.dir/requires: Source/Watershed/CMakeFiles/ITKWatershed.dir/ITKWatershed.o.requires
Source/Watershed/CMakeFiles/ITKWatershed.dir/requires: Source/Watershed/CMakeFiles/ITKWatershed.dir/moc_ITKWatershed.o.requires
.PHONY : Source/Watershed/CMakeFiles/ITKWatershed.dir/requires

Source/Watershed/CMakeFiles/ITKWatershed.dir/clean:
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Watershed && $(CMAKE_COMMAND) -P CMakeFiles/ITKWatershed.dir/cmake_clean.cmake
.PHONY : Source/Watershed/CMakeFiles/ITKWatershed.dir/clean

Source/Watershed/CMakeFiles/ITKWatershed.dir/depend: Source/Watershed/moc_ITKWatershed.cxx
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Watershed /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Watershed /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Watershed/CMakeFiles/ITKWatershed.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Source/Watershed/CMakeFiles/ITKWatershed.dir/depend

