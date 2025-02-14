#!/bin/bash
#THis is a shell progrem to batch build tracing methods plugins, including: 
#1.	All-path-pruning 1 (bottom-up pruning) @
#2.	All-path-pruning 2 (top-down pruning)  @ 
#3.	MOST  @
#4.	NeuTube
#5.	FarSight-Snake   @
#6.	SimpleTracing-dt  @
#7.	Simple-RayCasting @ 
#8.	Simple-RollingBall @
#9.	TreMap   @
#10.	MST-Tracing  @
#11.	NeuronGPSTree(HUST)   @
#12.	fastmarching_spanningtree(BJUT)  @
#13.	MeanshiftSpanningtree(BJUT)  @
#14.	CWlab_method1_version1(NTUST)  @
#15.	LCM_boost (A-STAR)   @ 
#16.	NeuroStalker (Sydney)  @
#17.	nctuTW(NCTUTW)  @
#18.	GD_nctuTW (ALLEN + NCTUTW)  @
#19.	SimpleAxisAnalyzer (NIU)  @
#20.	NeuronChaser(Erasmus)   @
#21.	smartTrace (ALLEN) @
#22.	neutu_autotrace (Janelia + PSU)  @
#23.	Advantra (Erasmus) @
#24.	RegMST (EPFL) @
#25.	EnsembleNeuronTracerBasic(NTUST)
#26.	EnsembleNeuronTracerV2n(NTUST)
#27.	EnsembleNeuronTracerV2s(NTUST)

#APP1 and APP2 
cd ../../../released_plugins/v3d_plugins/neurontracing_vn2/
make clean
svn up
qmake
make

#MOST 
cd ../neurontracing_MOST/
make clean
svn up
qmake
make	

#SimpleTracing-dt,  Simple-RollingBall, and Simple-RayCasting
cd ../neurontracing_simpletracing/
make clean
svn up
qmake
make

#smartTrace
cd ../neurontracing_smarttracing/
make clean
svn up
qmake
make

#TreMap 
cd  ../../../hackathon/zhi/neurontracing_mip/
make clean
svn up
qmake
make

#MST 
cd  ../../../hackathon/zhi/neurontracing_mst/
make clean
svn up
qmake
make

#snake
cd ../../../hackathon/zhi/snake_tracing/
if [ ! -d ITK_include ]; then
	unzip ITK_include.zip		
fi	
make clean
svn up
qmake
make

#NeuroGPSTree
cd ../../../bigneuron_ported/zhouhang_hust/NeuroGPSTree/
if [ ! -d eigen3.2.2 ]; then
	unzip eigen3.2.2	
fi	
make clean
svn up
qmake
make

#fastmarching_spanningtree 
cd ../../../bigneuron_ported/zhijiang_zn_bjut/fastmarching_spanningtree_vaa3d/
make clean
svn up
qmake
make

#MeanshiftSpanningtree 
cd ../../../bigneuron_ported/zhijiang_zn_bjut/MeanshiftSpanningtree/
make clean
svn up
qmake
make

#CWlab_method1_version1 
cd ../../../bigneuron_ported/chingwei/CWlab_method1_version1/
make clean
svn up
qmake
make

#LCM_boost 
cd ../../../bigneuron_ported/LinGU/LCM_boost/
make clean
svn up
qmake
make

#NeuroStalker 
cd ../../../bigneuron_ported/siqi/stalker_v3d/
make clean
svn up
qmake
make

#nctuTW 
cd ../../../bigneuron_ported/ytc/nctuTW/
make clean
svn up
qmake
make

#GD_nctuTW 
cd ../../../bigneuron_ported/ytc/nctuTW_tips/
make clean
svn up
qmake
make

#SimpleAxisAnalyzer 
cd ../../../bigneuron_ported/jiezhou/recon_3d_thin_v1/
make clean
svn up
qmake
make

#NeuronChaser
cd ../../../bigneuron_ported/miroslav_r/NeuronChaser/
make clean
svn up
qmake
make

#Advantra
cd ../../../bigneuron_ported/miroslav_r/Advantra/
make clean
svn up
qmake
make

#neutu_autotrace
cd ../../../bigneuron_ported/ting_dezhe/neutu_autotrace/
make clean
svn up
qmake
make

#RegMST
cd ../../../bigneuron_ported/AmosSironi_PrzemyslawGlowacki/RegMST_tracing_plugin/
make clean
svn up
qmake
make

cd ../../../bigneuron_ported/AmosSironi_PrzemyslawGlowacki/SQBTree_plugin/
make clean
svn up
qmake
make

#EnsembleNeuronTracerBasic
cd ../../../bigneuron_ported/chingwei/EnsembleNeuronTracerBasic/
make clean
svn up
qmake
make

#EnsembleNeuronTracerV2n
cd ../../../bigneuron_ported/chingwei/EnsembleNeuronTracerV2n/
make clean
svn up
qmake
make

#EnsembleNeuronTracerV2s
cd ../../../bigneuron_ported/chingwei/EnsembleNeuronTracerV2s/
make clean
svn up
qmake
make


#neuTube 
cd ../../../hackathon/zhi/neuTube_zhi/
make clean
svn up

cd ../../../released_plugins/v3d_plugins/neurontracing_neutube/src_neutube
if [ "$(uname)" == "Darwin" ]; then
  sh build.sh /usr/local/Trolltech/Qt-4.7.1/bin/qmake /usr/local/Trolltech/Qt-4.7.1/mkspecs/macx-g++42
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
	if [ ! -f ../../../released_plugins/v3d_plugins/neurontracing_neutube/src_neutube/neurolabi/c/lib/libneurolabi_debug.so ]; then
  		sh build.sh /usr/local/Trolltech/Qt-4.7.1/bin/qmake /usr/local/Trolltech/Qt-4.7.1/mkspecs/linux-g++
 		sh ../../../../hackathon/zhi/neuTube_zhi/update_library
	fi
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ]; then
  echo "detected windows platform" 
fi

cd ../../../../hackathon/zhi/neuTube_zhi/

qmake
make

mv ../../../../v3d_external/bin/plugins/NeuroStalker ../../../../v3d_external/bin/plugins/neuron_tracing/
mv ../../../../v3d_external/bin/plugins/LCM_boost ../../../../v3d_external/bin/plugins/neuron_tracing/
mv ../../../../v3d_external/bin/plugins/bigneuron_ported/* ../../../../v3d_external/bin/plugins/neuron_tracing/
rm -rf ../../../../v3d_external/bin/plugins/bigneuron_ported/
mv ../../../../v3d_external/bin/plugins/bigneuronhackathon/* ../../../../v3d_external/bin/plugins/neuron_tracing/
rm -rf ../../../../v3d_external/bin/plugins/bigneuronhackathon/




