
TEMPLATE	= lib
CONFIG	+= qt plugin warn_off
#CONFIG	+= x86_64
VAA3DPATH = ../../../../v3d_external/v3d_main
INCLUDEPATH	+= $$VAA3DPATH/basic_c_fun

HEADERS	+= IVSCC_process_swc_plugin.h
SOURCES	+= IVSCC_process_swc_plugin.cpp
SOURCES	+= $$VAA3DPATH/basic_c_fun/v3d_message.cpp
SOURCES	+= ../../../released_plugins/v3d_plugins/neurontracing_vn2/app2/my_surf_objs.cpp
SOURCES += $$VAA3DPATH/basic_c_fun/basic_surf_objs.cpp

TARGET	= $$qtLibraryTarget(IVSCC_process_swc)
DESTDIR	= $$VAA3DPATH/../bin/plugins/IVSCC/IVSCC_process_swc/
