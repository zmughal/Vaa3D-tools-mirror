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
include Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/depend.make

# Include the progress variables for this target.
include Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/progress.make

# Include the compile flags for this target's objects.
include Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/flags.make

Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/ThresholdMaximumConnectedComponents.o: Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/flags.make
Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/ThresholdMaximumConnectedComponents.o: /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Thresholding/ThresholdMaximumConnectedComponents.cxx
	$(CMAKE_COMMAND) -E cmake_progress_report /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/ThresholdMaximumConnectedComponents.o"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/ThresholdMaximumConnectedComponents.dir/ThresholdMaximumConnectedComponents.o -c /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Thresholding/ThresholdMaximumConnectedComponents.cxx

Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/ThresholdMaximumConnectedComponents.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ThresholdMaximumConnectedComponents.dir/ThresholdMaximumConnectedComponents.i"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Thresholding/ThresholdMaximumConnectedComponents.cxx > CMakeFiles/ThresholdMaximumConnectedComponents.dir/ThresholdMaximumConnectedComponents.i

Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/ThresholdMaximumConnectedComponents.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ThresholdMaximumConnectedComponents.dir/ThresholdMaximumConnectedComponents.s"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Thresholding/ThresholdMaximumConnectedComponents.cxx -o CMakeFiles/ThresholdMaximumConnectedComponents.dir/ThresholdMaximumConnectedComponents.s

Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/ThresholdMaximumConnectedComponents.o.requires:
.PHONY : Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/ThresholdMaximumConnectedComponents.o.requires

Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/ThresholdMaximumConnectedComponents.o.provides: Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/ThresholdMaximumConnectedComponents.o.requires
	$(MAKE) -f Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/build.make Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/ThresholdMaximumConnectedComponents.o.provides.build
.PHONY : Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/ThresholdMaximumConnectedComponents.o.provides

Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/ThresholdMaximumConnectedComponents.o.provides.build: Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/ThresholdMaximumConnectedComponents.o
.PHONY : Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/ThresholdMaximumConnectedComponents.o.provides.build

Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/moc_ThresholdMaximumConnectedComponents.o: Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/flags.make
Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/moc_ThresholdMaximumConnectedComponents.o: Source/Thresholding/moc_ThresholdMaximumConnectedComponents.cxx
	$(CMAKE_COMMAND) -E cmake_progress_report /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/moc_ThresholdMaximumConnectedComponents.o"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/ThresholdMaximumConnectedComponents.dir/moc_ThresholdMaximumConnectedComponents.o -c /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding/moc_ThresholdMaximumConnectedComponents.cxx

Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/moc_ThresholdMaximumConnectedComponents.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ThresholdMaximumConnectedComponents.dir/moc_ThresholdMaximumConnectedComponents.i"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding/moc_ThresholdMaximumConnectedComponents.cxx > CMakeFiles/ThresholdMaximumConnectedComponents.dir/moc_ThresholdMaximumConnectedComponents.i

Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/moc_ThresholdMaximumConnectedComponents.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ThresholdMaximumConnectedComponents.dir/moc_ThresholdMaximumConnectedComponents.s"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding/moc_ThresholdMaximumConnectedComponents.cxx -o CMakeFiles/ThresholdMaximumConnectedComponents.dir/moc_ThresholdMaximumConnectedComponents.s

Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/moc_ThresholdMaximumConnectedComponents.o.requires:
.PHONY : Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/moc_ThresholdMaximumConnectedComponents.o.requires

Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/moc_ThresholdMaximumConnectedComponents.o.provides: Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/moc_ThresholdMaximumConnectedComponents.o.requires
	$(MAKE) -f Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/build.make Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/moc_ThresholdMaximumConnectedComponents.o.provides.build
.PHONY : Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/moc_ThresholdMaximumConnectedComponents.o.provides

Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/moc_ThresholdMaximumConnectedComponents.o.provides.build: Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/moc_ThresholdMaximumConnectedComponents.o
.PHONY : Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/moc_ThresholdMaximumConnectedComponents.o.provides.build

Source/Thresholding/moc_ThresholdMaximumConnectedComponents.cxx: /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Thresholding/ThresholdMaximumConnectedComponents.h
	$(CMAKE_COMMAND) -E cmake_progress_report /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating moc_ThresholdMaximumConnectedComponents.cxx"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding && /usr/bin/moc-qt4 -I/home/liyun/V3D/v3d_main/basic_c_fun/../common_lib/include -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Review/Statistics -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Review -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/gdcm/src -I/home/liyun/ITK/ITK-bin/Utilities/gdcm -I/home/liyun/ITK/ITK-bin/Utilities/vxl/core -I/home/liyun/ITK/ITK-bin/Utilities/vxl/vcl -I/home/liyun/ITK/ITK-bin/Utilities/vxl/v3p/netlib -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/vxl/core -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/vxl/vcl -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/vxl/v3p/netlib -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities -I/home/liyun/ITK/ITK-bin/Utilities -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/itkExtHdrs -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/nifti/znzlib -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/nifti/niftilib -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/expat -I/home/liyun/ITK/ITK-bin/Utilities/expat -I/home/liyun/ITK/ITK-bin/Utilities/DICOMParser -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/DICOMParser -I/home/liyun/ITK/ITK-bin/Utilities/NrrdIO -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/NrrdIO -I/home/liyun/ITK/InsightToolkit-3.20.0/Utilities/MetaIO -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/SpatialObject -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Numerics/NeuralNetworks -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Numerics/FEM -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/IO -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Numerics -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Common -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/BasicFilters -I/home/liyun/ITK/InsightToolkit-3.20.0/Code/Algorithms -I/home/liyun/ITK/ITK-bin -I/usr/include/qt4 -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtCore -I/home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Common -I/home/liyun/V3D/v3d_main/basic_c_fun -I/home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins -DQT_GUI_LIB -DQT_CORE_LIB -o /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding/moc_ThresholdMaximumConnectedComponents.cxx /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Thresholding/ThresholdMaximumConnectedComponents.h

# Object files for target ThresholdMaximumConnectedComponents
ThresholdMaximumConnectedComponents_OBJECTS = \
"CMakeFiles/ThresholdMaximumConnectedComponents.dir/ThresholdMaximumConnectedComponents.o" \
"CMakeFiles/ThresholdMaximumConnectedComponents.dir/moc_ThresholdMaximumConnectedComponents.o"

# External object files for target ThresholdMaximumConnectedComponents
ThresholdMaximumConnectedComponents_EXTERNAL_OBJECTS =

bin/libThresholdMaximumConnectedComponents.so: Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/ThresholdMaximumConnectedComponents.o
bin/libThresholdMaximumConnectedComponents.so: Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/moc_ThresholdMaximumConnectedComponents.o
bin/libThresholdMaximumConnectedComponents.so: bin/libV3DInterface.a
bin/libThresholdMaximumConnectedComponents.so: bin/libV3DITKCommon.a
bin/libThresholdMaximumConnectedComponents.so: /usr/lib/libQtGui.so
bin/libThresholdMaximumConnectedComponents.so: /usr/lib/libQtCore.so
bin/libThresholdMaximumConnectedComponents.so: Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/build.make
bin/libThresholdMaximumConnectedComponents.so: Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX shared library ../../bin/libThresholdMaximumConnectedComponents.so"
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ThresholdMaximumConnectedComponents.dir/link.txt --verbose=$(VERBOSE)
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding && /usr/bin/cmake -E copy /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/bin/libThresholdMaximumConnectedComponents.so /home/liyun/V3D/v3d/plugins/ITK//Thresholding/ThresholdMaximumConnectedComponents/ThresholdMaximumConnectedComponents.so

# Rule to build all files generated by this target.
Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/build: bin/libThresholdMaximumConnectedComponents.so
.PHONY : Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/build

Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/requires: Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/ThresholdMaximumConnectedComponents.o.requires
Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/requires: Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/moc_ThresholdMaximumConnectedComponents.o.requires
.PHONY : Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/requires

Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/clean:
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding && $(CMAKE_COMMAND) -P CMakeFiles/ThresholdMaximumConnectedComponents.dir/cmake_clean.cmake
.PHONY : Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/clean

Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/depend: Source/Thresholding/moc_ThresholdMaximumConnectedComponents.cxx
	cd /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins /home/liyun/V3D/2010_hackathon/li_yun/ITK-V3D-Plugins/Source/Thresholding /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding /home/liyun/V3D/2010_hackathon/li_yun/ITK-Plugin-Build/Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Source/Thresholding/CMakeFiles/ThresholdMaximumConnectedComponents.dir/depend

