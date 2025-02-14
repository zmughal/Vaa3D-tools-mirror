
# Tiff and Ffftw libraries are required for istitch plugin

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(CMAKE_THREAD_PREFER_PTHREADS TRUE)
find_package(Threads REQUIRED)
if(NOT CMAKE_THREAD_LIBS_INIT)
    message("istitch plugin will not be built unless pthreads library is found")
endif()

if(TIFF_LIBRARY AND CMAKE_THREAD_LIBS_INIT AND FFTW_LIBRARY)
    include_directories( "${V3DMAINDIR}/basic_c_fun" )
    include_directories( "${TIFF_INCLUDE_DIR}" )
    include_directories( "${V3DMAINDIR}/common_lib/include" )
    include_directories( ${FFTW_INCLUDE_DIR})
    
    set(PLUGIN_HEADERS istitch.h istitch_gui.h )
    set(PLUGIN_DIRECTORY_NAME image_stitching/istitch)
    set(PLUGIN_SOURCES 
        istitch.cpp
        "${V3DMAINDIR}/basic_c_fun/stackutil.cpp"
        "${V3DMAINDIR}/basic_c_fun/mg_utilities.cpp"
        "${V3DMAINDIR}/basic_c_fun/mg_image_lib.cpp")
    
    configure_v3d_plugin(imageStitch)
    
    # Link to tiff
    target_link_libraries(imageStitch ${TIFF_LIBRARY})
    
    # Link to pthreads
    target_link_libraries(imageStitch ${CMAKE_THREAD_LIBS_INIT})
    
    # Link to fftw
    target_link_libraries(imageStitch ${FFTW_LIBRARY})
    target_link_libraries(imageStitch ${FFTW_THREADS_LIBRARY})
endif()

