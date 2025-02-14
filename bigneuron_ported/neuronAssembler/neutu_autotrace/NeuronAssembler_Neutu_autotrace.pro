
TEMPLATE	= lib
CONFIG	+= qt plugin warn_off
#CONFIG	+= x86_64
VAA3DPATH = ../../../../v3d_external/v3d_main/
INCLUDEPATH	+= $$VAA3DPATH/basic_c_fun
INCLUDEPATH	+= $$VAA3DPATH/common_lib/include

HEADERS	+= NeuronAssembler_Neutu_autotrace_plugin.h

SOURCES	+= NeuronAssembler_Neutu_autotrace_plugin.cpp
SOURCES	+= $$VAA3DPATH/basic_c_fun/v3d_message.cpp
SOURCES	+= $$VAA3DPATH/basic_c_fun/basic_surf_objs.cpp
SOURCES	+= $$VAA3DPATH/../released_plugins_more/v3d_plugins/neurontracing_vn2/app2/my_surf_objs.cpp
SOURCES	+= $$VAA3DPATH/basic_c_fun/stackutil.cpp
SOURCES	+= $$VAA3DPATH/basic_c_fun/mg_utilities.cpp
SOURCES	+= $$VAA3DPATH/basic_c_fun/mg_image_lib.cpp
SOURCES	+= $$VAA3DPATH/../../vaa3d_tools/hackathon/zhi/APP2_large_scale/readrawfile_func.cpp

LIBS	+= -lm -L$$VAA3DPATH/common_lib/lib -lv3dtiff
LIBS	+= -lpthread
LIBS	+= -lv3dfftw3f -lv3dfftw3f_threads
TARGET	= $$qtLibraryTarget(NeuronAssembler_Neutu_autotrace)
DESTDIR	= $$VAA3DPATH/../bin/plugins/neuron_tracing/NeuronAssembler_Neutu_autotrace/
