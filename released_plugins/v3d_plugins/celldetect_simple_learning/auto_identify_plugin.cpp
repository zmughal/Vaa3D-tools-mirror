/* auto_identify_plugin.cpp
 * It aims to automatically segment cells based on user-provided template markers;
 * 2012-01-01 : by XindiAi;
 * 2014-10-03 : by Xiang Li (lindbergh.li@gmail.com);
 */

#pragma region "headers and constant defination"
#include "v3d_message.h"
#include <vector>
#include <math.h>
#include "auto_identify_plugin.h"
#include "string"
#include "sstream"
#include "../../v3d_main/neuron_editing/v_neuronswc.h"
#include "../../v3d_main/neuron_tracing/neuron_tracing.h"
#include <time.h>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <typeinfo>
#include <fstream>

const double const_double_foregroundVoxelThreshold = 50;
const int const_int_preciseHistogram = 52;
const int const_int_smoothedHistogram = 26;
const int const_int_histogramBackground  = 5;
const int const_int_histogramDistanceForDeterminingValley = 2;
const int const_int_preciseHistogramInterval = 5;
const int const_int_smoothHistogramInterval = 10;
const int const_int_floatCriteria = 65534;
const int const_int_longCriteria = 254;
const double const_double_radiusMax = 5;
const double const_double_flatCriteria = 0.05;
const int const_int_maximumLoop = 60;
const double const_double_radiusIncrement = 0.5;
const double const_double_thresholdLoosedBy = 0.75;
const double const_double_radiusLoosedBy = 1.5;
const double const_double_smallRegionCriteria = 1;
const double const_double_largeRegionCriteria = 5;
const double const_double_maxPixelIntensity = 255;
const int const_int_maxIterations = 10000;
#define INF 1E9
#define PI 3.14159265
#pragma endregion

#pragma region "region grow, cut from regiongrow.ccp"
#include <stdlib.h>
#include <ctime>
#include <basic_surf_objs.h>
#include <volimg_proc.h>
#include <img_definition.h>
#include <basic_landmark.h>
using namespace std;

// extension to 3D from Hanchuan's 2D region growing codes
#define UBYTE unsigned char
#define BYTE signed char

volatile V3DLONG phcDebugPosNum = 0;
volatile V3DLONG phcDebugRgnNum = 0;

class POS
{
public:
    V3DLONG pos;
    V3DLONG order;
    POS * next;
    POS()
    {
        pos = -1;order=-1;
        next = 0;
        phcDebugPosNum++;
    }
    ~POS()
    {
        phcDebugPosNum--;
    }
};

class RGN
{
public:
    V3DLONG layer;
    V3DLONG no;
    POS *poslist;
    V3DLONG poslistlen;
    RGN * next;
    RGN()
    {
        layer=no=-1;
        poslistlen=0;poslist=0;
        next=0;
        phcDebugRgnNum++;
    }
    ~RGN()
    {
        layer=no=-1;
        poslistlen = 0;
        phcDebugRgnNum--;
    }
};

//statistics of count of labeling
class STCL
{
public:

    V3DLONG count;
    V3DLONG no;
    V3DLONG *desposlist;
    STCL *next;

    STCL()
    {
        count=no=-1;
        next=0;
        phcDebugRgnNum++;
    }
    ~STCL()
    {
        count=no=-1;
        phcDebugRgnNum--;
    }
};

//function of swap
template <class T>
void swap (T& x, T& y)
{
    T tmp = x;	x = y; y = tmp;
}

//function of quickSort
template <class T>
void quickSort(T a[], int l, int r)
{
    if(l>=r) return;
    int i = l;
    int j = r+1;

    T pivot = a[l];
    while(true)
    {
        do{ i = i+1; } while(a[i]>pivot);
        do{ j = j-1; } while(a[j]<pivot);
        if(i >= j) break;
        swap(a[i], a[j]);
    }
    a[l] = a[j];
    a[j] = pivot;
    quickSort(a, l, j-1);
    quickSort(a, j+1, r);
}

//memory management
template <class T> int newIntImage3dPairMatlabProtocol(T *** & img3d,T * & img1d, V3DLONG imgdep, V3DLONG imghei,V3DLONG imgwid)
{
    V3DLONG totalpxlnum = imghei*imgwid*imgdep;

    try
    {
        img1d = new T [totalpxlnum];
        img3d = new T ** [imgdep];

        V3DLONG i,j;

        for (i=0;i<imgdep;i++)
        {
            img3d[i] = new T * [imghei];
            for(j=0; j<imghei; j++)
                img3d[i][j] = img1d + i*imghei*imgwid + j*imgwid;

        }

        memset(img1d, 0, sizeof(T)*totalpxlnum);
    }
    catch(...)
    {
        if (img1d) {delete img1d;img1d=0;}
        if (img3d) {delete img3d;img3d=0;}
        printf("Fail to allocate mem in newIntImage2dPairMatlabProtocal()!");
        v3d_msg("Failed to allocate memory");
        return 0; //fail
    }

    return 1; //succeed
}

template <class T> void deleteIntImage3dPairMatlabProtocol(T *** & img3d,T * & img1d)
{
    if (img1d) {delete img1d;img1d=0;}
    if (img3d) {delete img3d;img3d=0;}
}

// region growing class
class RgnGrow3dClass
{
public:
    RgnGrow3dClass()
    {
        ImgWid = 0, ImgHei = 0, ImgDep = 0;
        quantImg1d=0; quantImg3d=0;
        PHCDONEIMG3d = 0, PHCDONEIMG1d = 0;

        STACKCNT = -1, MAXSTACKSIZE = 16, IFINCREASELABEL=-1, PHCURLABEL=-1;
        PHCLABELSTACK3d = 0, PHCLABELSTACK1d = 0;
        PHCLABELSTACKPOS = 0;

        PHCURGNPOS = 0, PHCURGNPOS_head = 0;
        PHCURGN = 0, PHCURGN_head = 0;
        TOTALPOSnum = 0, TOTALRGNnum = 0;
    }

    ~RgnGrow3dClass()
    {
        deleteIntImage3dPairMatlabProtocol(quantImg3d,quantImg1d);
        deleteIntImage3dPairMatlabProtocol(PHCLABELSTACK3d,PHCLABELSTACK1d);
        deleteIntImage3dPairMatlabProtocol(PHCDONEIMG3d,PHCDONEIMG1d);

        ImgWid = 0, ImgHei = 0, ImgDep = 0;

        STACKCNT = -1, MAXSTACKSIZE = 16, IFINCREASELABEL=-1, PHCURLABEL=-1;
        PHCLABELSTACKPOS = 0;

        PHCURGN = PHCURGN_head;
        for(V3DLONG i=0;i<TOTALRGNnum;i++)
        {
            RGN * pnextRgn = 0;
            if (PHCURGN)
            {
                pnextRgn = PHCURGN->next;
                PHCURGNPOS = PHCURGN->poslist;
                for(V3DLONG j=0;j<PHCURGN->poslistlen;j++)
                {
                    POS *pnextPos = 0;
                    if (PHCURGNPOS)
                    {
                        pnextPos = PHCURGNPOS->next;
                        delete PHCURGNPOS;
                    }
                    PHCURGNPOS = pnextPos;
                }
                delete PHCURGN;
            }
            PHCURGN = pnextRgn;
        }
        TOTALPOSnum = 0, TOTALRGNnum = 0;
    }

public:
    V3DLONG ImgWid, ImgHei, ImgDep;
    UBYTE * quantImg1d,  *** quantImg3d;
    BYTE *** PHCDONEIMG3d, * PHCDONEIMG1d;

    int STACKCNT;
    int MAXSTACKSIZE;
    int IFINCREASELABEL;
    V3DLONG PHCURLABEL;
    int ***PHCLABELSTACK3d, * PHCLABELSTACK1d;
    V3DLONG PHCLABELSTACKPOS;

    POS * PHCURGNPOS, * PHCURGNPOS_head;
    RGN * PHCURGN, * PHCURGN_head;

    V3DLONG TOTALPOSnum, TOTALRGNnum;
};

//generating an int image for any input image type
template <class T> void copyvecdata(T * srcdata, V3DLONG len, UBYTE * desdata, int& nstate, UBYTE &minn, UBYTE &maxx)
{
    if(!srcdata || !desdata)
    {
        printf("NULL pointers in copyvecdata()!\n");
        return;
    }

    V3DLONG i;

    //note: originally I added 0.5 before rounding, however seems the negative numbers and
    //      positive numbers are all rounded towarded 0; hence int(-1+0.5)=0 and int(1+0.5)=1;
    //      This is unwanted because I need the above to be -1 and 1.
    // for this reason I just round with 0.5 adjustment for positive and negative differently

    //copy data
    if (srcdata[0]>0)
        maxx = minn = int(srcdata[0]+0.5);
    else
        maxx = minn = int(srcdata[0]-0.5);

    int tmp;
    double tmp1;
    for (i=0;i<len;i++)
    {
        tmp1 = double(srcdata[i]);
        tmp = (tmp1>0)?(int)(tmp1+0.5):(int)(tmp1-0.5);//round to integers
        minn = (minn<tmp)?minn:tmp;
        maxx = (maxx>tmp)?maxx:tmp;
        desdata[i] = (UBYTE)tmp;
    }
    maxx = (UBYTE)maxx;
    minn = (UBYTE)minn;

    //return the #state
    nstate = (maxx-minn+1);

    return;
}

void rgnfindsub(int rowi,int colj, int depk, int direction,int stackinc, RgnGrow3dClass * pRgnGrow)
{
    if (pRgnGrow->STACKCNT >= pRgnGrow->MAXSTACKSIZE)
    {
        if (pRgnGrow->IFINCREASELABEL != 0)
            pRgnGrow->IFINCREASELABEL = 0;
        return;
    }

    BYTE *** flagImg = pRgnGrow->PHCDONEIMG3d;
    int ImgWid = pRgnGrow->ImgWid;
    int ImgHei = pRgnGrow->ImgHei;
    int ImgDep = pRgnGrow->ImgDep;

    if (stackinc==1)
    {
        pRgnGrow->PHCLABELSTACK3d[0][0][pRgnGrow->PHCLABELSTACKPOS] = depk;
        pRgnGrow->PHCLABELSTACK3d[0][1][pRgnGrow->PHCLABELSTACKPOS] = colj;
        pRgnGrow->PHCLABELSTACK3d[0][2][pRgnGrow->PHCLABELSTACKPOS] = rowi;

        pRgnGrow->STACKCNT++;
        pRgnGrow->PHCLABELSTACKPOS++;

        flagImg[depk][colj][rowi] = -1;//pRgnGrow->PHCDONEIMG2d[coli][rowj] = -1;
        //PHCLABELIMG[coli][rowj] = PHCURLABEL;

        //set the current pos location and return the
        if (pRgnGrow->PHCURGNPOS)
        {
            pRgnGrow->PHCURGNPOS->pos = (V3DLONG) depk*(pRgnGrow->ImgHei * pRgnGrow->ImgWid) + colj*(pRgnGrow->ImgWid) + rowi; 
            pRgnGrow->PHCURGNPOS->next = new POS;
            if (pRgnGrow->PHCURGNPOS->next==0)
            {printf("Fail to do: pRgnGrow->PHCURGNPOS->next = new POS; -->current phcDebugPosNum=%i.\n",phcDebugPosNum);}
            pRgnGrow->PHCURGNPOS = pRgnGrow->PHCURGNPOS->next;
            pRgnGrow->TOTALPOSnum++;
            //printf("current pixel: row=%i col=%i total_pxl_visted=%i \n",rowj,coli,pRgnGrow->TOTALPOSnum);
        }
        else
        {
            printf("PHCURGNPOS is null!!\n");
        }
    }
    else //%if stackinc==0,
    {
        //pRgnGrow->PHCDONEIMG[coli][rowj] = -2;
        flagImg[depk][colj][rowi] = -2;
    }

    // % search 26 direction orders

    // 1
    if (rowi>0 && flagImg[depk][colj][rowi-1]==1)
        rgnfindsub(rowi-1,colj,depk,1,1,pRgnGrow);
    // 2
    if (rowi<ImgWid-1 && flagImg[depk][colj][rowi+1]==1)
        rgnfindsub(rowi+1,colj,depk,1,1,pRgnGrow);
    // 3
    if (colj>0 && flagImg[depk][colj-1][rowi]==1)
        rgnfindsub(rowi,colj-1,depk,1,1,pRgnGrow);
    // 4
    if (colj<ImgHei-1 && flagImg[depk][colj+1][rowi]==1)
        rgnfindsub(rowi,colj+1,depk,1,1,pRgnGrow);
    // 5
    if (depk>0 && flagImg[depk-1][colj][rowi]==1)
        rgnfindsub(rowi,colj,depk-1,1,1,pRgnGrow);
    // 6
    if (depk<ImgDep-1 && flagImg[depk+1][colj][rowi]==1)
        rgnfindsub(rowi,colj,depk+1,1,1,pRgnGrow);
    // 7
    if (rowi>0 && colj>0 && flagImg[depk][colj-1][rowi-1]==1)
        rgnfindsub(rowi-1,colj-1,depk,1,1,pRgnGrow);
    // 8
    if (rowi<ImgWid-1 && colj>0 && flagImg[depk][colj-1][rowi+1]==1)
        rgnfindsub(rowi+1,colj-1,depk,1,1,pRgnGrow);
    // 9
    if (rowi>0 && colj<ImgHei-1 && flagImg[depk][colj+1][rowi-1]==1)
        rgnfindsub(rowi-1,colj+1,depk,1,1,pRgnGrow);
    // 10
    if (rowi>ImgWid && colj<ImgHei-1 && flagImg[depk][colj+1][rowi+1]==1)
        rgnfindsub(rowi+1,colj+1,depk,1,1,pRgnGrow);
    // 11
    if (rowi>0 && depk>0 && flagImg[depk-1][colj][rowi-1]==1)
        rgnfindsub(rowi-1,colj,depk-1,1,1,pRgnGrow);
    // 12
    if (rowi<ImgWid-1 && depk>0 && flagImg[depk-1][colj][rowi+1]==1)
        rgnfindsub(rowi+1,colj,depk-1,1,1,pRgnGrow);
    // 13
    if (rowi>0 && depk<ImgDep-1 && flagImg[depk+1][colj][rowi-1]==1)
        rgnfindsub(rowi-1,colj,depk+1,1,1,pRgnGrow);
    // 14
    if (rowi<ImgWid-1 && depk<ImgDep-1 && flagImg[depk+1][colj][rowi+1]==1)
        rgnfindsub(rowi+1,colj,depk+1,1,1,pRgnGrow);
    // 15
    if (colj>0 && depk>0 && flagImg[depk-1][colj-1][rowi]==1)
        rgnfindsub(rowi,colj-1,depk-1,1,1,pRgnGrow);
    // 16
    if (colj<ImgHei-1 && depk>0 && flagImg[depk-1][colj+1][rowi]==1)
        rgnfindsub(rowi,colj+1,depk-1,1,1,pRgnGrow);
    // 17
    if (colj>0 && depk<ImgDep-1 && flagImg[depk+1][colj-1][rowi]==1)
        rgnfindsub(rowi,colj-1,depk+1,1,1,pRgnGrow);
    // 18
    if (colj<ImgHei-1 && depk<ImgDep-1 && flagImg[depk+1][colj+1][rowi]==1)
        rgnfindsub(rowi,colj+1,depk+1,1,1,pRgnGrow);
    // 19
    if (rowi>0 && colj>0 && depk>0 && flagImg[depk-1][colj-1][rowi-1]==1)
        rgnfindsub(rowi-1,colj-1,depk-1,1,1,pRgnGrow);
    // 20
    if (rowi<ImgWid-1 && colj>0 && depk>0 && flagImg[depk-1][colj-1][rowi+1]==1)
        rgnfindsub(rowi+1,colj-1,depk-1,1,1,pRgnGrow);
    // 21
    if (rowi>0 && colj<ImgHei-1 && depk>0 && flagImg[depk-1][colj+1][rowi-1]==1)
        rgnfindsub(rowi-1,colj+1,depk-1,1,1,pRgnGrow);
    // 22
    if (rowi>0 && colj>0 && depk<ImgDep-1 && flagImg[depk+1][colj-1][rowi-1]==1)
        rgnfindsub(rowi-1,colj-1,depk+1,1,1,pRgnGrow);
    // 23
    if (rowi<ImgWid-1 && colj<ImgHei-1 && depk>0 && flagImg[depk-1][colj+1][rowi+1]==1)
        rgnfindsub(rowi+1,colj+1,depk-1,1,1,pRgnGrow);
    // 24
    if (rowi<ImgWid-1 && colj>0 && depk<ImgDep-1 && flagImg[depk+1][colj-1][rowi+1]==1)
        rgnfindsub(rowi+1,colj-1,depk+1,1,1,pRgnGrow);
    // 25
    if (rowi>0 && colj<ImgHei-1 && depk<ImgDep-1 && flagImg[depk+1][colj+1][rowi-1]==1)
        rgnfindsub(rowi-1,colj+1,depk+1,1,1,pRgnGrow);
    // 26
    if (rowi<ImgWid-1 && colj<ImgHei-1 && depk<ImgDep-1 && flagImg[depk+1][colj+1][rowi+1]==1)
        rgnfindsub(rowi+1,colj+1,depk+1,1,1,pRgnGrow);


    return;
}

bool regiongrowing(V3DPluginCallback2 &callback, int c, double thresh, double rad,
                   LandmarkList &cmList, vector<int> &volList,
                   unsigned char* &outimg8, unsigned short* &outimg16, float* &outimg32);
#pragma endregion

#pragma region "UI-related functions"
Q_EXPORT_PLUGIN2(auto_identify, AutoIdentifyPlugin);

#define V_NeuronSWC_list vector<V_NeuronSWC>

//controlPanel_SWC* controlPanel_SWC::m_pLookPanel_SWC = 0;

void count_cells(V3DPluginCallback2 &callback, QWidget *parent);
void identify_neurons(V3DPluginCallback2 &callback, QWidget *parent);

template <class T> bool identify_cells(V3DPluginCallback2 &callback, T* data1d, V3DLONG *dimNum, int c,
                                       const LandmarkList & markerlist, LandmarkList & bglist,
                                       LandmarkList & outputlist, Image4DSimple &maskImage,
                                       int &cat, double &rad, double &val, double thresh);

bool neuron_2_mark(const NeuronTree & p, LandmarkList & neuronMarkList);
template <class T> int pixelVal(T* data1d, V3DLONG *dimNum,
                                double xc, double yc, double zc, int c);
template <class T> bool mass_center_Lists(T* data1d,
                                    V3DLONG *dimNum,
                                    LandmarkList & originalList, LandmarkList & newList,
                                    double radius, int c, double thresh);
template <class T> bool mass_center_Coords(T* data1d,
                                    V3DLONG *dimNum,
                                    int &x, int &y, int &z,
                                    double radius, int c, double thresh);
template <class T> bool mass_center_Masks(unsigned char* data1d, T maskImg, T rgnImg, V3DLONG *dimNum,
                                          double rad, int &xc, int &yc, int &zc, int c, int rgn);
template <class T> double compute_ave_cell_val(T* data1d, V3DLONG *dimNum,
                                        double xc,double yc,double zc,int c,double rad);
template <class T> bool compute_cell_values_rad(T* data1d,
                                                V3DLONG *dimNum,
                                                double xc, double yc, double zc,
                                                int c, double threshold, double & outputdataAve,
                                                double & outputrad, double & outputZrad);
template <class T> LandmarkList scan_and_count(T* data1d, V3DLONG *dimNum,
                                               double cellAve, double cellStDev,
                                               double PointAve, double PointStDev,
                                               double radAve, double radStDev,
                                               int c, int cat, double thresh,
                                               LandmarkList originalList, Image4DSimple &maskImage);
template <class T> LandmarkList remove_duplicates(T* data1d, LandmarkList fullList,
                                           V3DLONG *dimNum, int PointAve, int rad, int c);
template <class T> bool type_duplicates(T* data1d, LandmarkList &mList,
                                                V3DLONG *dimNum, int c,
                                                vector<double> catArr, vector<double> radArr, vector<double> valArr);
V_NeuronSWC get_v_neuron_swc(const NeuronTree *p);
V_NeuronSWC_list get_neuron_segments(const NeuronTree *p);
NeuronTree VSWC_2_neuron_tree(V_NeuronSWC *p, int id);
NeuronSWC make_neuron_swc(V_NeuronSWC_unit *p, int id, bool &start);
template <class T> bool compute_swc_radius(T* data1d, V3DLONG *dimNum, vector<V_NeuronSWC_unit> segment,
                                           int c, double & outputRadAve, double & outputRadStDev);
bool export_list2file(QList<NeuronTree> & N2, QString fileSaveName, QString fileOpenName);
template <class T> bool apply_mask(unsigned char* data1d, V3DLONG *dimNum,
                                   int xc, int yc, int zc, int c, double threshold, T & maskImg);
bool open_testSWC(QString &fileOpenName, NeuronTree & openTree);
template <class T> bool segment_regions(unsigned char* data1d, V3DLONG *dimNum,
                                        LandmarkList inputList,
                                        LandmarkList &newList,
                                        LandmarkList regionList,
                                        vector<int> regionVol,
                                        double radAve, double radStDev, double radZ,
                                        int c, int cat, double thresh,
                                        T* regionData);
template <class T> LandmarkList seg_by_mask (unsigned char* data1d, T* rgnData, unsigned char* maskData,
                                             vector<int> cell_template, V3DLONG *dimNum,
                                             LocationSimple cellLocation, int radAve, int radZ,
                                             double range, int cellcount, int rgn, int c, int thresh,
                                             double Xmean, double Sx);
 
QStringList AutoIdentifyPlugin::menulist() const
{
    return QStringList()
        <<tr("Cell Counting")
        <<tr("Label Neurons")
		<<tr("about");
}

QStringList AutoIdentifyPlugin::funclist() const
{
	return QStringList()
		<<tr("func1")
		<<tr("func2")
		<<tr("help");
}

void AutoIdentifyPlugin::domenu(const QString &menu_name, V3DPluginCallback2 &callback, QWidget *parent)
{
    if (menu_name == tr("Cell Counting"))
	{
        count_cells(callback,parent);
    }
    else if (menu_name == tr("Label Neurons"))
    {
        identify_neurons(callback,parent);
    }
	else
	{
        v3d_msg(tr("Uses current image's landmarks to find similar objects in image."
            "Work-in-process by Xindi, 2014 Summer"
			"Updated by Xiang Li (lindbergh.li@gmail.com)"
			));
	}
}

bool AutoIdentifyPlugin::dofunc(const QString & func_name, const V3DPluginArgList & input, V3DPluginArgList & output, V3DPluginCallback2 & callback,  QWidget * parent)
{
	vector<char*> infiles, inparas, outfiles;
	if(input.size() >= 1) infiles = *((vector<char*> *)input.at(0).p);
	if(input.size() >= 2) inparas = *((vector<char*> *)input.at(1).p);
	if(output.size() >= 1) outfiles = *((vector<char*> *)output.at(0).p);

	if (func_name == tr("func1"))
	{
		
	}
	else if (func_name == tr("func2"))
	{
		
	}
	else if (func_name == tr("help"))
	{
		
	}
	else return false;

	return true;
}
#pragma endregion

#pragma region "identify neurons interface"
/*  ##################################
 * [completed tasks]
 * test neuron SWC needs to be its own structure marked with the comment "test"
 * radius calculation of branches works
 * average intensity of branches works
 * categorizing non-test data sets based on test data works
 * saving newly labeled SWC file works
 * radius StDev of branches works
 * can load test data directly from file
 * newly labeled SWC is now automatically added to 3D window
 * Better UI implimented
 *
 * [current goals/issues]
 * some coords are returning as indeterminant in the compute_radius function. Right now they are being skipped. Not sure source of bad numbers.
 *
 * [future goals]
 * optimize radius calculation algorithm to be more robust, figure out why it's even capable of returning bad numbers
 *  ##################################
*/
void identify_neurons(V3DPluginCallback2 &callback, QWidget *parent)
{
    v3dhandle curwin = callback.currentImageWindow();

    //cancels if no image
    if (!curwin)
    {
        v3d_msg("You don't have any image open in the main window.");
        return;
    }

    //if image, pulls the data
    Image4DSimple* p4DImage = callback.getImage(curwin); //the data of the image is in 4D (channel + 3D)

    unsigned char* data1d = p4DImage->getRawData(); //sets data into 1D array

    QString curfilename = callback.getImageName(curwin);

    //defining the dimensions
    V3DLONG N = p4DImage->getXDim();
    V3DLONG M = p4DImage->getYDim();
    V3DLONG P = p4DImage->getZDim();
    V3DLONG sc = p4DImage->getCDim();

    //storing the dimensions
    V3DLONG dimNum[4];
    dimNum[0]=N; dimNum[1]=M; dimNum[2]=P; dimNum[3]=sc;

    QList<NeuronTree> * mTreeList;
    mTreeList = callback.getHandleNeuronTrees_3DGlobalViewer(curwin);
    NeuronTree mTree;
    if (mTreeList->isEmpty()) { v3d_msg("There are no neuron traces in the current window."); return; }
    else
    {
        vector<int> segCatArr(10);
        vector<double> segRadArr(10);
        vector<double> segIntensArr(10);
        vector<double> segRadStDevArr(10);

        //take user inputs
        Dialog_SWC dialog(callback, parent, sc);
        if (dialog.exec()!=QDialog::Accepted)
        {
            return;
        }

        int c = dialog.channel;

        QString infileName = dialog.infileName;

        NeuronTree openTree;
        if (open_testSWC(infileName,openTree))
        {
            openTree.comment = "test";
            mTreeList->append(openTree);
        }
        int structNum = mTreeList->count();

        //get examples from test data
        int check = 0;
        for (int i=0; i<structNum; i++)
        {
            mTree = mTreeList->at(i);

            if (mTree.comment != "test") //defining the testing set by comments
            {
                check++;
                continue;
            }
            
            V_NeuronSWC_list seg_list = get_neuron_segments(&mTree);
            //syntax: list.at(i) is segment, segment.row is vector of units, vector.at(i) is unit, unit.type is category
            int segNum = seg_list.size();
            for (int j=0; j<segNum; j++)
            {
                //segCatArr.push_back(seg_list.at(j).row.at(0).type);
                int segCat = seg_list.at(j).row.at(0).type;
                segCatArr[segCat] += 1;
                double radAve,radStDev;
                compute_swc_radius(data1d,dimNum,seg_list.at(j).row,c,radAve,radStDev);
                double x,y,z,intensity=0;
                for (int k=0; k<seg_list.at(j).row.size(); k++)
                {
                    x=seg_list.at(j).row.at(k).x;
                    y=seg_list.at(j).row.at(k).y;
                    z=seg_list.at(j).row.at(k).z;
                    intensity += pixelVal(data1d,dimNum,x,y,z,c);
                }
                double intensAve = intensity/seg_list.at(j).row.size();
                segRadArr[segCat] += radAve;
                segIntensArr[segCat] += intensAve;
                segRadStDevArr[segCat] += radStDev;
            }
            for (int k=0; k<segCatArr.size(); k++)
            {
                if (segCatArr[k]==0) continue;
                segRadArr[k] /= segCatArr[k];
                segIntensArr[k] /= segCatArr[k];
                segRadStDevArr[k] /= segCatArr[k];
            }
        }
        if (check==structNum)
        {
            v3d_msg("No exampler data found, please specify a set of example data and try again.");
            return;
        }
        //label remaining neurons using test data
        NeuronTree newTree;
        QList<NeuronTree> newTreeList;
        for (int i=0; i<structNum; i++) //loops through neuron structures as numbered in object manager
        {
            mTree = mTreeList->at(i);

            if (mTree.comment == "test") continue; //defining the testing set by comments
            
            V_NeuronSWC_list seg_list = get_neuron_segments(&mTree);
            //syntax: list.at(i) is segment, segment.row is vector of units, vector.at(i) is unit, unit.type is category
            int segNum = seg_list.size();
            int id=1;
            for (int j=0; j<segNum; j++) //loops through segments within one structure
            {
                double radAve,radStDev;
                compute_swc_radius(data1d,dimNum,seg_list.at(j).row,c,radAve,radStDev);
                double x,y,z,intensity=0;
                for (int k=0; k<seg_list.at(j).row.size(); k++)
                {
                    x=seg_list.at(j).row.at(k).x;
                    y=seg_list.at(j).row.at(k).y;
                    z=seg_list.at(j).row.at(k).z;
                    intensity += pixelVal(data1d,dimNum,x,y,z,c);
                }
                double intensAve = intensity/seg_list.at(j).row.size();
                double diffRad,diffInt,diffRadStD,diff,diff_min=255;
                int cur_type=3;
                for (int k=0; k<segCatArr.size(); k++)
                {
                    if (segCatArr.at(k)==0) continue;
                    diffRad = abs(radAve-segRadArr.at(k));
                    diffInt = abs(intensAve-segIntensArr.at(k));
                    diffRadStD = abs(radStDev-segRadStDevArr.at(k));
                    diff = (2*diffRad+diffInt+3*diffRadStD)/6; //depending on further testing, may end up weighing this average differently
                    if (diff<diff_min)
                    {
                        cur_type=k;
                        diff_min=diff;
                    }
                }
                for (int l=0; l<seg_list.at(j).row.size(); l++)
                {
                    seg_list.at(j).row.at(l).type=cur_type; //sets every unit in segment to be new type
                }
                newTree = VSWC_2_neuron_tree(&seg_list.at(j),id); //translates segment into a NeuronTree
                id += seg_list.at(j).row.size();
                newTreeList.append(newTree);
            }

        }
        QString outfilename = dialog.outfileName;
        string SaveName = curfilename.toStdString();
        unsigned int a = SaveName.rfind('.');
        unsigned int b = SaveName.rfind('/');
        SaveName = SaveName.substr(b+1,a-b-1);
        SaveName.append("_Labeled_SWC.swc");
        outfilename.append("/" + QString::fromStdString(SaveName));
        export_list2file(newTreeList,outfilename,curfilename);
        NeuronTree nt = readSWC_file(outfilename);
        callback.setSWC(curwin, nt);
        callback.updateImageWindow(curwin);
    }
    return;
}
#pragma endregion

#pragma region "count cells interface"
/*  ##################################
 * [completed tasks]
 * all main algorithms are functional, may not be optimized
 * added in use of corner voxels as negative exemplars
 * improved threshold algorithm
 * significantly reduced duplicate detection of cells
 * mass center algorithm inaccuracy fixed
 * mass center should be smarter now
 * fixed apply_mask, broke mass_center...again
 * implemented new UI
 * implemented new algorithm based on region growing
 * mass_center actually fixed now
 * added exemplar cell_template for template matching in seg_by_mask
 * added separate radius detection for flatter images
 *
 * [current goals/issues]
 * histogram threshold algorithm has trouble with high-intensity or large-volume noise
 * type sorting threshold is having issues with accurate voxel value detection
 *
 * [future goals]
 *
 *  ##################################
*/
void count_cells(V3DPluginCallback2 &callback, QWidget *parent)
{
    v3dhandle curwin = callback.currentImageWindow();

    //cancels if no image
    if (!curwin)
    {
        v3d_msg("You don't have any image open in the main window.");
        return;
    }

    //if image, pulls the data
    Image4DSimple* p4DImage = callback.getImage(curwin); //the data of the image is in 4D (channel + 3D)

    unsigned char* data1d = p4DImage->getRawData(); //sets data into 1D array

    //defining the dimensions
    V3DLONG N = p4DImage->getXDim();
    V3DLONG M = p4DImage->getYDim();
    V3DLONG P = p4DImage->getZDim();
    V3DLONG sc = p4DImage->getCDim();

    //storing the dimensions
    V3DLONG dimNum[4];
    dimNum[0]=N; dimNum[1]=M; dimNum[2]=P; dimNum[3]=sc;

    LandmarkList Marklist = callback.getLandmark(curwin);
    int Marknum = Marklist.count();
    if (Marknum>0)
    {
        //default marker cat as 1 for type sorting
        for (int i=0; i<Marklist.count(); i++)
        {
            LocationSimple tmp = Marklist.at(i);
            if (tmp.comments.empty()==true) tmp.category=1;
            else
            {
                istringstream cat(tmp.comments);
                cat>>tmp.category;
            }
            Marklist.replace(i,tmp);
        }
    }

    QList<NeuronTree> * mTreeList=0;
    mTreeList = callback.getHandleNeuronTrees_3DGlobalViewer(curwin);

    //check data availability
    if (Marknum<=0 && !mTreeList)
    {
        v3d_msg("you have not specified any marker or swc structure to run this program.");
        return;
    }

    int SWCcount;
    NeuronTree mTree;
    if(mTreeList)
    {
        if (mTreeList->isEmpty()) { SWCcount = 0; }
        else
        {
                mTree = mTreeList->first();
                SWCcount = mTree.listNeuron.count();
        }
    }

    //input test data type
    int option;
    if (Marknum != 0 && SWCcount ==0 ) { option = 1; }
    else if (Marknum == 0 && SWCcount != 0) { option = 2; }
    else {option == 0;}

    Dialog_cells dialog(callback, parent,sc,option);
    if (dialog.exec()!=QDialog::Accepted)
    {
        return;
    }

    option = dialog.input_type;
    LandmarkList mlist, neuronMarkList;
    if (option == 1) //input is markers
    {
        mlist = Marklist;
    }
    else if (option == 2 ) //input is SWC
    {
        neuron_2_mark(mTree,neuronMarkList);
        mlist = neuronMarkList;
    }
    else    //input is both markers and SWC
    {
        neuron_2_mark(mTree,neuronMarkList);
        mlist = Marklist;
        mlist.append(neuronMarkList);
    }
    int c = dialog.channel;
    int sort_type = dialog.sort_type;
    double thresh = dialog.thresh;
    if (thresh==0) {v3d_msg("Please provide a valid threshold"); return;}

    if (sort_type==0) //binary sort
    {
        if (mlist.count()<=0)    {v3d_msg("There are no markers in the current image"); return;}

        int start_t = clock();
        LandmarkList bglist; //sending in empty bglist to trigger binary sort
        LandmarkList outputList;
        Image4DSimple maskImage;
        int cat;
        double rad,val;
        if (identify_cells(callback,data1d,dimNum,c,mlist,bglist,outputList,maskImage,cat,rad,val,thresh))
        {
            int end_t = clock();
            cout<<"time elapse ..."<<end_t-start_t<<endl;
            v3d_msg(QString("Final Cell Count: %1").arg(outputList.count()));
            callback.setLandmark(curwin,outputList);
        }
    }
    else if (sort_type==1) //type sort
    {
        int * catList;
        catList = new int[mlist.count()];
        if (mlist.count()<=0)    {v3d_msg("There are no neuron traces or markers in the current image"); return;}
        int start_t = clock();
        LocationSimple tempInd;
        for (int i=0; i<mlist.count(); i++)
        {
            tempInd = mlist.at(i);
            catList[i] = tempInd.category;
        }

        //counts number of categories
        int catNum=0;
        for (int i=0; i<mlist.count(); i++)
        {
            for (int j=0; j<mlist.count(); j++)
            {
                if (catList[j]==i)
                {
                    catNum++;
                    break;
                }
            }
        }
        if (catNum<=1) {v3d_msg("Not enough categories. Do nothing."); return;}
        map<int,LandmarkList> catArr;
        LandmarkList temp;
        int row=0;
        for (int catval=0; catval<10; catval++) //loop through category values
        {
            int x=0;
            for (int index=0; index<mlist.count(); index++) //loop through markers
            {
                if (catList[index]==catval)
                {
                    x++;
                    temp.append(mlist.at(index));
                }
            }
            if (x==0)
                continue;
            else
            {
                catArr.insert(make_pair(row,temp));
                row++;
                temp.clear();
            }
        }
        if (catList) {delete [] catList; catList=0;}

        LandmarkList catSortList;
        LandmarkList * marks;
        LandmarkList * bgs = &catArr[0]; //working with assumption that bg has category value 0;
        if (bgs->isEmpty()) //if background category not specified by user, use corner voxels as bg
        {
            for (int i=1; i<=N; i+=N-1)
            {
                for (int j=1; j<=M; j+=M-1)
                {
                    for (int k=1; k<=P; k+=P-1)
                    {
                        if (data1d[(c-1)*P*M*N+(k-1)*M*N+(j-1)*N+(i-1)]>const_double_foregroundVoxelThreshold) continue;
                        LocationSimple extraBG(i,j,k);
                        extraBG.category = 0;
                        bgs->append(extraBG);
                    }
                }
            }
        }
        vector<double> catArr2,radArr,valArr;

        for (int i=0; i<catNum-1; i++)
        {
            marks = &catArr[i+1];
            LandmarkList tempList;
            Image4DSimple maskImage;
            int cat;
            double rad,val;
            cout<<endl<<"counting cells of category "<<i+1<<endl;
            if (identify_cells(callback,data1d,dimNum,c,*marks,*bgs,tempList,maskImage,cat,rad,val,thresh))
                catSortList.append(tempList);
            catArr2.push_back(cat);
            radArr.push_back(rad);
            valArr.push_back(val);
        }
        if (catNum>2) type_duplicates(data1d,catSortList,dimNum,c,catArr2,radArr,valArr);

        int end_t = clock();
        cout<<"time elapse ..."<<end_t-start_t<<endl;
        v3d_msg(QString("Final Cell Count: %1").arg(catSortList.count()));
        callback.setLandmark(curwin,catSortList);

    }
    return;
}
#pragma endregion

#pragma region "count cells main: FUNCTION: identify_cells"
template <class T> bool identify_cells(V3DPluginCallback2 &callback, T* data1d, V3DLONG *dimNum, int c,
                                       const LandmarkList & markerlist, LandmarkList & bglist,
                                       LandmarkList & outputlist, Image4DSimple &maskImage,
                                       int &cat, double &rad, double &val, double thresh)
{
	if (!data1d || !dimNum)
        return false;

	//ofstream logFile;
	//logFile.open ("log_identify_cells.txt");

    V3DLONG N = dimNum[0];
    V3DLONG M = dimNum[1];
    V3DLONG P = dimNum[2];

    LandmarkList mlist, MarkList, BGList;
    LocationSimple tmpLocation(0,0,0);
    int xc,yc,zc, marks;
    double PixVal,BGVal;
    bool catsorting=false;
    if (bglist.isEmpty()) //binary sorting
    {
        catsorting=false;
        //sort markers by background/foreground

        //add corner pixels to list to use as additional bg markers
        mlist = markerlist;
        for (int i=1; i<=N; i+=N-1)
        {
            for (int j=1; j<=M; j+=M-1)
            {
                for (int k=1; k<=P; k+=P-1)
                {
                    if (data1d[(c-1)*P*M*N+(k-1)*M*N+(j-1)*N+(i-1)]>const_double_foregroundVoxelThreshold) continue;
                    LocationSimple extraBG(i,j,k);
                    extraBG.category = 0;
                    mlist.append(extraBG);
                }
            }
        }
        int pix,num;
        int marknum = mlist.count();
        int * PixValArr=0;
        PixValArr = new int[marknum];
        for (int i=0; i<marknum; i++)
        {
            tmpLocation = mlist.at(i);
            tmpLocation.getCoord(xc,yc,zc);
            pix=data1d[(c-1)*P*M*N+(zc-1)*M*N+(yc-1)*N+(xc-1)];
            PixValArr[i] = pix;
        }
        if (thresh < 0)
        {
			//threshold calculation based on histogram of marker+bg voxel intensities
            vector<int> pValHist(const_int_preciseHistogram,0); //precise histogram for getting threshold value
            vector<int> pValHist_smooth(const_int_smoothedHistogram,0); //smoothed histogram for finding valley
            pValHist[0]=const_int_histogramBackground;
			pValHist_smooth[0]=const_int_histogramBackground; //initalize background values for thresh calc
            for (int i=0; i<=marknum; i++)
            {
                int histInd = PixValArr[i]/const_int_preciseHistogramInterval;
                int smoothHistInd = PixValArr[i]/const_int_smoothHistogramInterval;
                if (histInd<0) histInd=0;
                if (histInd>(const_int_preciseHistogram-1)) histInd=(const_int_preciseHistogram-1);
                if (smoothHistInd<0) smoothHistInd=0;
                if (smoothHistInd>(const_int_smoothedHistogram-1)) smoothHistInd=(const_int_smoothedHistogram-1);
                num = pValHist[histInd];
                pValHist[histInd] = num+1;
                num = pValHist_smooth[smoothHistInd];
                pValHist_smooth[smoothHistInd] = num+1;
            }
			vector<int> localMaxs;
            if (pValHist_smooth[0]>pValHist_smooth[1]) localMaxs.push_back(0);
            int num0,num1,num2;
            for (int i=1; i<=(const_int_smoothedHistogram-2); i++)
            {
                num0=pValHist_smooth[i-1];
                num1=pValHist_smooth[i];
                num2=pValHist_smooth[i+1];
                if (num1>0 && num1>=num0 && num1>=num2) {localMaxs.push_back(i);}
            }
            if (pValHist_smooth[const_int_smoothedHistogram-1]>pValHist_smooth[const_int_smoothedHistogram-2]) localMaxs.push_back(const_int_smoothedHistogram-1);
            int max=const_int_histogramDistanceForDeterminingValley,valleyPos=0,diff=0;
            for (int i=0; i<localMaxs.size()-1; i++)
            {
                diff = localMaxs[i+1]-localMaxs[i];
                if (diff>max) {valleyPos=i;}
            }
            valleyPos *= 2;
            if (valleyPos<=0) valleyPos=1;
            if (valleyPos>=(const_int_preciseHistogram-1)) valleyPos=(const_int_preciseHistogram-2);
            if (pValHist[valleyPos-1]==pValHist[valleyPos]==pValHist[valleyPos+1]) thresh=valleyPos*const_int_preciseHistogramInterval;
            int end=valleyPos+diff*2;
            if (end>(const_int_preciseHistogram-2)) end=(const_int_preciseHistogram-2);
            for (int i=valleyPos; i<=end; i++)
            {
                num0=pValHist[i-1];
                num1=pValHist[i];
                num2=pValHist[i+1];
                if (num1<=num0 && num1<=num2) {thresh=i*const_int_preciseHistogramInterval;}
            }
        }
        cout<<"threshold value "<<thresh<<endl;
		
        PixVal=0, BGVal=0;
        for (int i=0; i<marknum; i++)
        {
            num=PixValArr[i];
            tmpLocation = mlist.at(i);
            if (num<thresh) { BGList.append(tmpLocation); BGVal += num; }    //BGList holds bg markers
            if (num>thresh) { MarkList.append(tmpLocation); PixVal += num; }  //MarkList holds cell markers
        }

        PixVal = PixVal/MarkList.count();   //PixVal now stores average pixel value of all cell markers
        if (!BGList.isEmpty()) BGVal = BGVal/BGList.count();
        else BGVal = 0;                     //BGVal now stores average pixel value of all background markers
    }
    else    //comment sorting
	{
        catsorting=true;
        MarkList = markerlist;
        PixVal=0, BGVal=0;
        double markmin=const_double_maxPixelIntensity, bgmax=0;
        for (int i=0; i<MarkList.count(); i++)
        {
            tmpLocation = MarkList.at(i);
            tmpLocation.getCoord(xc,yc,zc);
            int pix = pixelVal(data1d,dimNum,xc,yc,zc,c);
            PixVal += pix;
            if (pix<markmin) markmin=pix;
        }
        BGList = bglist;
        for (int i=0; i<BGList.count(); i++)
        {
            tmpLocation = BGList.at(i);
            tmpLocation.getCoord(xc,yc,zc);
            int pix = pixelVal(data1d,dimNum,xc,yc,zc,c);
            BGVal += pix;
            if (pix>bgmax) bgmax=pix;
        }

        PixVal = PixVal/MarkList.count();   //PixVal now stores average pixel value of all cell markers
        BGVal = BGVal/BGList.count();       //BGVal now stores average pixel value of all background markers
        if (thresh<0)
        {
            if (bgmax<=markmin) thresh = (int)((markmin+bgmax)/2 + 0.5);
            else thresh = (int)((PixVal+BGVal)/2 + 0.5);
        }
        cout<<"threshold value "<<thresh<<endl;
    }
	marks = MarkList.count();
    if (MarkList.count()<=0)
    {
        v3d_msg("Marker list is empty. Quit.");
        return false;
    }
    cat = MarkList.at(0).category;
    //recalibrates input marker list by mean shift
    LandmarkList CenteredList;
    LocationSimple tmpcent(0,0,0);
    int x,y,z;
    for (int i=0; i<marks; i++)
    {
        double blah,checkrad=0,checkZrad=0;
		tmpcent = MarkList.at(i);
        tmpcent.getCoord(x,y,z);
        compute_cell_values_rad(data1d,dimNum,x,y,z,c,thresh,blah,checkrad,checkZrad);
        if (checkrad<const_double_radiusMax) {checkrad=const_double_radiusMax;}
        mass_center_Coords(data1d,dimNum,x,y,z,checkrad*1.5,c,thresh);
        tmpcent.x=x; tmpcent.y=y; tmpcent.z=z;
        CenteredList.append(tmpcent);
    }
	//logFile <<"# of markers: "<<marks<<"; Calibration succeed!!!"<<endl;
    MarkList = CenteredList;
    //scan list of cell markers for ValAve, radAve
    int * ValAveArr; int * radAveArr; int * radZAveArr;
    ValAveArr = new int[marks]; radAveArr = new int[marks]; radZAveArr = new int[marks];
    LocationSimple tempLocation(0,0,0);
    double ValAve=0,radAve=0,radZAve=0,count=0;
    double tmpDataAve,tmpRad,tmpZRad,tmpDataAveImg,tmpDataAveRgn,tmpRadImg,tmpRadRgn;
    for (int i=0; i<marks; i++)
    {
        tempLocation = MarkList.at(i);
        tempLocation.getCoord(xc,yc,zc);
        if(compute_cell_values_rad(data1d,dimNum,xc,yc,zc,c,thresh,tmpDataAve,tmpRad,tmpZRad))
        {
            ValAveArr[i] = tmpDataAve;
            radAveArr[i] = tmpRad;
            radZAveArr[i] = tmpZRad;
            ValAve += ValAveArr[i];
            radAve += radAveArr[i];            
            radZAve += radZAveArr[i];
            count++;
        }
    }
	if (count < 1)
	{
		ValAve = 0;
		radAve = 0;
		radZAve = 0;
		v3d_msg("Warning: error in estimating model paramters, please select the markers again!");
		return false;
	}
	else
	{
		ValAve /= count;  //average pixel value of each cell
		radAve /= count;  //average radius of cell
		radZAve /= count; //average z-thickness for flat image cells
	}
    double stV=0, stR=0, stP=0;
    for (int i=0; i<marks; i++)
    {
        double s = (ValAveArr[i]-ValAve)*(ValAveArr[i]-ValAve);
        stV += s;

        double t = (radAveArr[i]-radAve)*(radAveArr[i]-radAve);
        stR += t;

        tempLocation = MarkList.at(i);
        tempLocation.getCoord(xc,yc,zc);
        int Pix = pixelVal(data1d,dimNum,xc,yc,zc,c);
        double u = (Pix-PixVal)*(Pix-PixVal);
        stP += u;
    }
    double ValStDev = sqrt(stV/marks);
    double radStDev = sqrt(stR/marks);
    double PixStDev = sqrt(stP/marks);
	//logFile <<"Estimation of intensity: "<<ValAve<<"+-"<<ValStDev<<"; radius: "<<radAve<<"+-"<<"; Zradius: "<<radZAve<<"; pixelStdev: "<<PixStDev<<"!!!"<<endl;

    //pulls region list from regiongrow plugin
    LandmarkList regionList;
    vector<int> regionVol;
    unsigned char* rgn8 = 0;
    unsigned short* rgn16 = 0;
    float* rgn32 = 0;
    if (!regiongrowing(callback,c,thresh,radAve,regionList,regionVol,(unsigned char* &)rgn8,(unsigned short* &)rgn16,(float* &)rgn32)) return false;
	//logFile <<"Region growing succeed!!!"<<endl;

    if (regionList.size()>const_int_floatCriteria)
        segment_regions(data1d,dimNum,MarkList,outputlist,regionList,regionVol,radAve,radStDev,radZAve,c,cat,thresh,rgn32);
    else if (regionList.size()>const_int_longCriteria)
        segment_regions(data1d,dimNum,MarkList,outputlist,regionList,regionVol,radAve,radStDev,radZAve,c,cat,thresh,rgn16);
    else
        segment_regions(data1d,dimNum,MarkList,outputlist,regionList,regionVol,radAve,radStDev,radZAve,c,cat,thresh,rgn8);
	//logFile <<"Segmentation (segment_regions) succeed!!!"<<endl;

    if (catsorting==true) //set same category and color
    {
        srand(clock()); 
        RGBA8 col = random_rgba8(255);
        for (int i=0; i<outputlist.size();i++)
        {
            LocationSimple tmp(0,0,0);
            tmp = outputlist.at(i);
            tmp.category = cat;
            tmp.color = col;
            stringstream catStr;
            catStr << cat;
            tmp.comments = catStr.str();
            outputlist.replace(i,tmp);
        }
    }
    rad = radAve;
    val = PixVal;

	//logFile <<"Done!!!"<<" Cell count: "<<outputlist.count()<<" based on "<<marks<<" exemplar cells!!!"<<endl;
    return true;
}
#pragma endregion







//returns pixel value of marker
template <class T> int pixelVal(T* data1d, V3DLONG *dimNum,
                                double xc, double yc, double zc, int c)
{
    V3DLONG N = dimNum[0];
    V3DLONG M = dimNum[1];
    V3DLONG P = dimNum[2];
	int pixelIndex =  (V3DLONG)(c-1)*P*M*N + (V3DLONG)(zc-1)*M*N + (V3DLONG)(yc-1)*N + (V3DLONG)(xc-1) ;
	if (pixelIndex < 0) {return INF;}
	if (pixelIndex > (N*M*P-1)) {return INF;}
    return data1d[pixelIndex];
}


//returns recentered LandmarkList
template <class T> bool mass_center_Lists(T* data1d, V3DLONG *dimNum,
                                          LandmarkList & originalList, LandmarkList & newList,
                                          double radius, int c, double thresh)
{
    V3DLONG N = dimNum[0];
    V3DLONG M = dimNum[1];
    V3DLONG P = dimNum[2];

    double pVal,newX,newY,newZ,norm;
    double x,y,z,pi=3.14;
    int xc,yc,zc;
    LocationSimple tmp(0,0,0);
    for (int i=0; i<originalList.size(); i++)
    {
        double rad = radius;
        tmp = originalList.at(i);
        tmp.getCoord(xc,yc,zc);
        int xo=xc,yo=yc,zo=zc;
        int check=0,runs=0;
        bool converge=false,bad_data=false;
        
        do
        {
            pVal = data1d[(c-1)*P*M*N+(V3DLONG)(zc-1)*M*N+(V3DLONG)(yc-1)*N+(V3DLONG)(xc-1)];
            newX=xc*pVal, newY=yc*pVal, newZ=zc*pVal, norm=pVal;
            for (double r=rad/5; r<=rad; r+=rad/5)
            {
                for (double theta=0; theta<2*pi; theta+=(pi/8))
                {
                    for (double phi=0; phi<pi; phi+=(pi/8))
                    {
                        x = xc+r*cos(theta)*sin(phi) + 0.5;
                        y = yc+r*sin(theta)*sin(phi) + 0.5;
                        z = zc+r*cos(phi) + 0.5;
                        if (x<1 || x>N || y<1 || y>M || z<1 || z>P) continue;
                        pVal = data1d[(c-1)*P*M*N+(V3DLONG)(z-1)*M*N+(V3DLONG)(y-1)*N+(V3DLONG)(x-1)];
                        if (pVal>=thresh)
                        {
                            newX += pVal*x;
                            newY += pVal*y;
                            newZ += pVal*z;
                            norm += pVal;
                        }
                    }
                }
            }
            if (norm<=0) {bad_data=true; goto stop;}
            newX /= norm; newX+=0.5;
            newY /= norm; newY+=0.5;
            newZ /= norm; newZ+=0.5;
            
            if ((int)newX==xc && (int)newY==yc && (int)newZ==zc) {converge=true;}
            xc=(int)newX; yc=(int)newY; zc=(int)newZ;
            runs++;
        } while (converge==false && runs<10); //if not converged in 10 runs, likely stuck in rounding loop
        stop:;

        if (bad_data==true) {newX=xo; newY=yo; newZ=zo;}

        if (newX<0) newX=0; if (newX>N-1) newX=N-1;
        if (newY<0) newY=0; if (newY>M-1) newY=M-1;
        if (newZ<0) newZ=0; if (newZ>P-1) newZ=P-1;

        LocationSimple newMark(newX,newY,newZ);
        newList.append(newMark);
    }
    return true;
}

//returns recentered coords x,y,z
template <class T> bool mass_center_Coords(T* data1d, V3DLONG *dimNum,
                                           int &x, int &y, int &z,
                                           double radius, int c, double thresh)
{
    LandmarkList tmpList,recenteredList;
    LocationSimple tmp(x,y,z),recentered(0,0,0);
    tmpList.append(tmp);
    mass_center_Lists(data1d,dimNum,tmpList,recenteredList,radius,c,thresh);
    recentered = recenteredList.at(0);
    x = recentered.x;
    y = recentered.y;
    z = recentered.z;
    return true;
}

//returns recentered coords taking masks into account
template <class T> bool mass_center_Masks(unsigned char* data1d, T maskImg, T rgnImg, V3DLONG *dimNum,
                                          double rad, int &xc, int &yc, int &zc, int c, int rgn)
{
    V3DLONG N = dimNum[0];
    V3DLONG M = dimNum[1];
    V3DLONG P = dimNum[2];

    double pVal,newX,newY,newZ,norm;
    double x,y,z,pi=3.14;
    int runs=0;
    bool converge=false;
    do
    {
        pVal = pixelVal(data1d,dimNum,xc,yc,zc,c);
        newX=xc*pVal, newY=yc*pVal, newZ=zc*pVal, norm=pVal;
        int count=0,hit=0;
        for (double r=rad/5; r<=rad; r+=rad/5)
        {
            for (double theta=0; theta<2*pi; theta+=(pi/8))
            {
                for (double phi=0; phi<pi; phi+=(pi/8))
                {
                    x = xc+r*cos(theta)*sin(phi);
                    if (x>N-1) x=N-1; if (x<0) x=0;
                    y = yc+r*sin(theta)*sin(phi);
                    if (y>M-1) y=M-1; if (y<0) y=0;
                    z = zc+r*cos(phi);
                    if (z>P-1) z=P-1; if (z<0) z=0;
                    if (*rgnImg.at(x,y,z,0)==rgn)
                    {
                        count++;
                        if (*maskImg.at(x,y,z,0)==0)
                        {
                            pVal = pixelVal(data1d,dimNum,x,y,z,c);
                            newX += pVal*x;
                            newY += pVal*y;
                            newZ += pVal*z;
                            norm += pVal;
                            hit++;
                        }
                    }
                }
            }
        }
        if (hit/count<0.5) return false;
        newX /= norm;
        newY /= norm;
        newZ /= norm;

        if (newX==xc && newY==yc && newZ==zc) {converge=true;}

        xc=newX; yc=newY; zc=newZ;

        runs++;
    } while (converge==false && runs<10); //if not converged in 10 runs, likely stuck in rounding loop

    return true;
}

//returns average pixel value of shell of computed radius around a marker
template <class T> bool compute_cell_values_rad(T* data1d,
                                                V3DLONG *dimNum,
                                                double xc, double yc, double zc,
                                                int c, double threshold, double & outputdataAve,
                                                double & outputrad, double & outputZrad)
{
    V3DLONG N = dimNum[0];
    V3DLONG M = dimNum[1];
    V3DLONG P = dimNum[2];
    outputrad=0; outputZrad=0;
	int iterations = 0;

	if (P/N <= 0.05 && P/M <= 0.05) //if image is flat, calculate 2D circle radius and estimate z-thickness
    {
		iterations = 0;
        do
        {
			iterations++;
			if (iterations > const_int_maxIterations) 
			{
				v3d_msg("warning: maximum iterations reached during radius estimation!");
				break;
			}
            outputrad+=0.5;
            double x,y,z=zc;
            int runs=0,datatotal=0;
            for (double theta=0; theta<2*PI; theta+=(PI/8))
            {
                x = xc+outputrad*cos(theta) +0.5;
                if (x>N) x=N; if (x<1) x=1;
                y = yc+outputrad*sin(theta) +0.5;
                if (y>M) y=M; if (y<1) y=1;
                int dataval = data1d[(c-1)*P*M*N+(V3DLONG)(z-1)*M*N+(V3DLONG)(y-1)*N+(V3DLONG)(x-1)];
                datatotal += dataval;
                runs++;
            }
            runs++;
            outputdataAve = datatotal/runs;
        } while ( outputdataAve > threshold*3/4 );

        double zmin_C=0,zmax_C=0,minDist=0,maxDist=0;
        int zmin_V=0,zmax_V=0;
		iterations = 0;
        do
        {
			iterations++;
			if (iterations > const_int_maxIterations) 
			{
				v3d_msg("warning: maximum iterations reached during z-radius(-) estimation!");
				break;
			}
            minDist+=0.5;
            if ((zc-minDist)<=0) {minDist=zc; break;}
            zmin_C = zc-minDist;
            zmin_V = pixelVal(data1d,dimNum,xc,yc,zmin_C,c);
			if (zmin_V == INF)
			{
				v3d_msg("warning: function pixelVal() returns INF, possibly due to invalid access address;");
				break;
			}
        } while ( zmin_V > threshold );

		iterations = 0;
		do
        {
			iterations++;
			if (iterations > const_int_maxIterations) 
			{
				v3d_msg("warning: maximum iterations reached during z-radius(+) estimation!");
				break;
			}
            maxDist+=0.5;
            if (zc+maxDist>P-1) {maxDist=P-zc-1; break;}
            zmax_C = zc+maxDist;
            zmax_V = pixelVal(data1d,dimNum,xc,yc,zmax_C,c);
			if (zmin_V == INF)
			{
				v3d_msg("warning: function pixelVal() returns INF, possibly due to invalid access address;");
				break;
			}
        } while ( zmax_V > threshold );
		
        if (minDist<0) minDist=0;
        if (maxDist<0) maxDist=0;
        outputZrad = (minDist+maxDist)/2;
    }
    else //if image is not flat, calculate 3D sphere radius
    {
        do
        {
            outputrad+=0.5;
            double x,y,z;
            int runs=0,datatotal=0;
            for (double theta=0; theta<2*PI; theta+=(PI/8))
            {
                for (double phi=0; phi<PI; phi+=(PI/8))
                {
                    x = xc+outputrad*cos(theta)*sin(phi) +0.5;
                    if (x>N) x=N; if (x<1) x=1;
                    y = yc+outputrad*sin(theta)*sin(phi) +0.5;
                    if (y>M) y=M; if (y<1) y=1;
                    z = zc+outputrad*cos(phi) +0.5;
                    if (z>P) z=P; if (z<1) z=1;
                    int dataval = data1d[(c-1)*P*M*N+(V3DLONG)(z-1)*M*N+(V3DLONG)(y-1)*N+(V3DLONG)(x-1)];
                    datatotal += dataval;
                    runs++;
                }
            }
            runs++;
            outputdataAve = datatotal/runs;
        } while ( outputdataAve > threshold*3/4 );
    }

    return true;
}


//returns average pixel value of sphere of radius rad on channel c around a given marker
template <class T> double compute_ave_cell_val(T* data1d, V3DLONG *dimNum,
                                               double xc, double yc, double zc, int c, double rad)
{
    V3DLONG N = dimNum[0];
    V3DLONG M = dimNum[1];
    V3DLONG P = dimNum[2];

    double x,y,z,datatotal=0,pi=3.14;
    int runs=0;

    for (double r=rad/5; r<=rad; r+=rad/5)
    {
        for (double theta=0; theta<2*pi; theta+=(pi/4))
        {
            for (double phi=0; phi<pi; phi+=(pi/4))
            {
                x = xc+r*cos(theta)*sin(phi);
                if (x>N) x=N; if (x<1) x=1;
                y = yc+r*sin(theta)*sin(phi);
                if (y>M) y=M; if (y<1) y=1;
                z = zc+r*cos(phi);
                if (z>P) z=P; if (z<1) z=1;
                int dataval = data1d[(c-1)*P*M*N+(int)(z-1)*M*N+(int)(y-1)*N+(int)(x-1)];
                datatotal += dataval;
                runs++;
            }
        }
    }
    double dataAve;
    if (runs==0) dataAve=0;
    else dataAve = datatotal/runs;

    return dataAve;
}


//uses test data to scan and mark other cells
template <class T> LandmarkList scan_and_count(T* data1d, V3DLONG *dimNum,
                                               double cellAve, double cellStDev,
                                               double PointAve, double PointStDev,
                                               double radAve, double radStDev,
                                               int c, int cat, double thresh,
                                               LandmarkList originalList, Image4DSimple &maskImage)
{
    V3DLONG N = dimNum[0];
    V3DLONG M = dimNum[1];
    V3DLONG P = dimNum[2];

    //set up mask
    unsigned char *maskData = 0;
    maskData = new unsigned char [N*M*P];
    for (V3DLONG tmpi=0;tmpi<N*M*P;++tmpi) maskData[tmpi] = 0; //preset to be all 0
    maskImage.setData((unsigned char*)maskData, N, M, P, 1, V3D_UINT8);
    Image4DProxy<Image4DSimple> maskImg(&maskImage);

    LandmarkList newList;
    cout<<"starting count"<<endl;

    //set original markers into new list and mask them
    if (!originalList.empty())
    {
        for (int i=0; i<originalList.size(); i++)
        {
            LocationSimple tmp(0,0,0);
            int xc,yc,zc;
            tmp = originalList.at(i);
            tmp.getCoord(xc,yc,zc);
            apply_mask(data1d,dimNum,xc,yc,zc,c,thresh,maskImg);
        }
        newList = originalList;
    }

    LocationSimple tmpLocation(0,0,0);
    double seg;
    seg = radAve/4;
    double init=radAve+radStDev;
    double end=radAve-radStDev;
    if (end<1) { end=1;}
    for (double i=init; i>=end; i-=0.5)
    {
        for (double iz=seg; iz<P; iz+=seg)
        {
            for (double iy=seg; iy<M; iy+=seg)
            {
                for (double ix=seg; ix<N; ix+=seg)
                {
                    if (*maskImg.at(ix,iy,iz,0) == 0)
                    {
                        double TempPointVal = pixelVal(data1d,dimNum,ix,iy,iz,c);
                        if ((TempPointVal>=PointAve-PointStDev) && (TempPointVal<=PointAve+PointStDev))
                        {
                            int x,y,z;
                            x=ix; y=iy; z=iz; //mass_center_Coords will rewrite x,y,z so need to keep ix,iy,iz untouched
                            mass_center_Coords(data1d,dimNum,x,y,z,radAve+radStDev,c,thresh);
                            if (*maskImg.at(x,y,z,0)!=0) continue;
                            double TempDataAve = compute_ave_cell_val(data1d,dimNum,x,y,z,c,i); //this algorithm has been changed from shell to volume, no longer accurate for this usage
                            if ( (TempDataAve>=cellAve-cellStDev) && (TempDataAve<=cellAve+cellStDev))
                            {
                                tmpLocation.x = x;
                                tmpLocation.y = y;
                                tmpLocation.z = z;
                                tmpLocation.category = cat;
                                stringstream catStr;
                                catStr << cat;
                                tmpLocation.comments = catStr.str();
                                newList.append(tmpLocation);

                                apply_mask(data1d,dimNum,x,y,z,c,thresh,maskImg);
                            }
                        }
                    }
                }
            }
        }
    }
    return newList;
}


//detects markers too close together, deletes marker with pixel value farther from PointAve
template <class T> LandmarkList remove_duplicates(T* data1d, LandmarkList fullList,
                                           V3DLONG *dimNum, int PointAve, int rad, int c)
{
    cout<<"Removing Duplicates"<<endl;
    int marknum = fullList.count();
    LandmarkList smallList = fullList;
    int x1,y1,z1,x2,y2,z2;
    double data1,data2,t,dist;
    LocationSimple point1(0,0,0), point2(0,0,0);
    LocationSimple zero(0,0,0);
    LocationSimple& zer = zero;
    for (int i=0; i<marknum; i++)
    {
        for (int j=i+1; j<marknum; j++)
        {
            point1 = smallList.at(i);
            if (point1==zer) continue;
            point1.getCoord(x1,y1,z1);
            int pix1 = pixelVal(data1d,dimNum,x1,y1,z1,c);
            point2 = smallList.at(j);
            if (point2==zer) continue;
            point2.getCoord(x2,y2,z2);
            int pix2 = pixelVal(data1d,dimNum,x2,y2,z2,c);

            t = (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2);
            dist = sqrt(t);
            if (dist<=rad)
            {
                data1 = abs(pix1-PointAve);
                data2 = abs(pix2-PointAve);
                if (data1>=data2)
                    smallList.replace(i,zer) ;//replace point1 with 0 to avoid changing length of list and messing up indexes
                else
                    smallList.replace(j,zer) ;//replace point2
            }
        }
    }
    smallList.removeAll(zer);
    return smallList;
}

//removes duplicate markers from type overlap
template <class T> bool type_duplicates(T* data1d, LandmarkList &mList,
                                                V3DLONG *dimNum, int c,
                                                vector<double> catArr, vector<double> radArr, vector<double> valArr)
{
    cout<<"Removing Duplicates"<<endl;
    int marknum = mList.count();
    int x1,y1,z1,x2,y2,z2;
    double data1,data2,t,dist;
    LocationSimple point1(0,0,0), point2(0,0,0);
    LocationSimple zero(0,0,0);
    LocationSimple& zer = zero;
    for (int i=0; i<marknum; i++)
    {
        for (int j=i+1; j<marknum; j++)
        {
            point1 = mList.at(i);
            if (point1==zer) continue;
            point1.getCoord(x1,y1,z1);
            int pix1 = pixelVal(data1d,dimNum,x1,y1,z1,c);
            double blah1,rad1,radZ1;
            compute_cell_values_rad(data1d,dimNum,x1,y1,z1,c,pix1/2,blah1,rad1,radZ1);
            string catStr1 = point1.comments;
            stringstream convert1(catStr1);
            int cat1,pos1;
            convert1 >> cat1;
            for (int i=0; i<catArr.size(); i++)
            {
                if (catArr[i]==cat1) {pos1=i; break;}
            }

            point2 = mList.at(j);
            if (point2==zer) continue;
            point2.getCoord(x2,y2,z2);
            int pix2 = pixelVal(data1d,dimNum,x2,y2,z2,c);
            double blah2,rad2,radZ2;
            compute_cell_values_rad(data1d,dimNum,x2,y2,z2,c,pix1/2,blah2,rad2,radZ2);
            string catStr2 = point2.comments;
            stringstream convert2(catStr2);
            int cat2,pos2;
            convert2 >> cat2;
            for (int i=0; i<catArr.size(); i++)
            {
                if (catArr[i]==cat2) {pos2=i; break;}
            }

            t = (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2);
            dist = sqrt(t);
            if (dist<=radArr[pos1] || dist<=radArr[pos2])
            {
                data1 = (abs(pix1-valArr[pos1])+abs(rad1-radArr[pos1]))/2;
                data2 = (abs(pix2-valArr[pos2])+abs(rad2-radArr[pos2]))/2;
                if (data1>=data2)
                    mList.replace(i,zer) ;//replace point1 with 0 to avoid changing length of list and messing up indexes
                else
                    mList.replace(j,zer) ;//replace point2
            }
        }
    }
    mList.removeAll(zer);
    cout<<endl<<"Final Cell Count: "<<mList.count()<<endl;
    return true;
}

bool neuron_2_mark(const NeuronTree & p, LandmarkList & neuronMarkList)
{
    LocationSimple tmpMark(0,0,0);
    for (int i=0;i<p.listNeuron.size();i++)
    {
        tmpMark.x = p.listNeuron.at(i).x;
        tmpMark.y = p.listNeuron.at(i).y;
        tmpMark.z = p.listNeuron.at(i).z;
        tmpMark.category = p.listNeuron.at(i).type;
        neuronMarkList.append(tmpMark);
    }
    return true;
}

V_NeuronSWC get_v_neuron_swc(const NeuronTree *p)
{
    V_NeuronSWC cur_seg;	cur_seg.clear();
    const QList<NeuronSWC> & qlist = p->listNeuron;

    for (V3DLONG i=0;i<qlist.size();i++)
    {
        V_NeuronSWC_unit v;
        v.n		= qlist[i].n;
        v.type	= qlist[i].type;
        v.x 	= qlist[i].x;
        v.y 	= qlist[i].y;
        v.z 	= qlist[i].z;
        v.r 	= qlist[i].r;
        v.parent = qlist[i].pn;

        cur_seg.append(v);
    }
    cur_seg.name = qPrintable(QString("%1").arg(1));
    cur_seg.b_linegraph=true; //donot forget to do this
    return cur_seg;
}
V_NeuronSWC_list get_neuron_segments(const NeuronTree *p)
{
    V_NeuronSWC cur_seg = get_v_neuron_swc(p);
    V_NeuronSWC_list seg_list;
    seg_list = cur_seg.decompose();
    return seg_list;
}
NeuronSWC make_neuron_swc(V_NeuronSWC_unit *p, int id, bool &start)
{
    NeuronSWC N;

    N.n     = id;
    N.type  = p->type;
    N.x     = p->x;
    N.y     = p->y;
    N.z     = p->z;
    N.r     = p->r;
    if (start==false)    N.parent = id-1;
    else                 N.parent = -1;

    return N;
}

NeuronTree VSWC_2_neuron_tree(V_NeuronSWC *p, int id)
{
    QList<NeuronSWC> nTree;
    bool start;
    for (int j=0; j<p->row.size(); j++)
    {
        if (j==0)   start=true;
        else        start=false;
        V_NeuronSWC_unit v = p->row.at(j);
        NeuronSWC n = make_neuron_swc(&v,id,start);
        nTree.append(n);
        id++;
    }

    NeuronTree newTree;
    newTree.listNeuron = nTree;
    return newTree;
}

//compute radius of neuron SWC segment
template <class T> bool compute_swc_radius(T* data1d, V3DLONG *dimNum, vector<V_NeuronSWC_unit> segment,
                                           int c, double & outputRadAve, double & outputRadStDev)
{
    V3DLONG N = dimNum[0];
    V3DLONG M = dimNum[1];
    V3DLONG P = dimNum[2];
    double rad,radAve,radTot=0;
    vector<double> radArr;
    int init,end;
    if (segment.size()>2)       {init=1;end=segment.size()-1;} //going to omit first and last unit per segment
    else if (segment.size()==2) {init=0;end=1;}
    else {v3d_msg("Neuron segment with only 1 node, shouldn't be there"); return false;}
    for (int unit=init; unit<end; unit++)
    {
        V_NeuronSWC_unit P1,P2,P0;

        if (segment.size()>2)
        {
            P0 = segment.at(unit);
            P1 = segment.at(unit-1);
            P2 = segment.at(unit+1);
        }
        if (segment.size()==2)
        {
            P1 = segment.at(0);
            P2 = segment.at(1);
            P0.x = (P2.x+P1.x)/2;
            P0.y = (P2.y+P1.y)/2;
            P0.z = (P2.z+P1.z)/2;
        }

        double Vnum[] = {P2.x-P1.x,P2.y-P1.y,P2.z-P1.z};
        double Vnorm  = sqrt(Vnum[0]*Vnum[0]+Vnum[1]*Vnum[1]+Vnum[2]*Vnum[2]);
        double V[]    = {Vnum[0]/Vnorm,Vnum[1]/Vnorm,Vnum[2]/Vnorm}; //axis of rotation
        double Anum[] = {-V[1],V[0],(-V[1]*V[0]-V[0]*V[1])/V[2]};
        double Anorm  = sqrt(Anum[0]*Anum[0]+Anum[1]*Anum[1]+Anum[2]*Anum[2]);
        double A[]    = {Anum[0]/Anorm,Anum[1]/Anorm,Anum[2]/Anorm}; //perpendicular to V
        double B[]    = {A[1]*V[2]-A[2]*V[1],A[2]*V[0]-A[0]*V[2],A[0]*V[1]-A[1]*V[0]}; //perpendicular to A and V
        rad=0;
        int pValCircTot=0, runs=0;
        int pVal = pixelVal(data1d,dimNum,P0.x,P0.y,P0.z,c);
        if (pVal<const_double_foregroundVoxelThreshold)
        {
            rad+=0.1;
            continue;
        }
        else
        {
            double check=255, pi=3.14;
            float x,y,z;
            do
            {
                rad += 0.2;
                for (double theta=0; theta<2*pi; theta+=pi/8)
                {
                    x = P0.x+rad*cos(theta)*A[0]+rad*sin(theta)*B[0];
                    if (x>N-1) x=N-1; if (x<0) x=0;
                    y = P0.y+rad*cos(theta)*A[1]+rad*sin(theta)*B[1];
                    if (y>M-1) y=M-1; if (y<0) y=0;
                    z = P0.z+rad*cos(theta)*A[2]+rad*sin(theta)*B[2];
                    if (z>P-1) z=P-1; if (z<0) z=0;
                    if (x!=x || y!=y || z!=z) continue; //skips segments that return indefinite coords
                    double pValCirc = pixelVal(data1d,dimNum,x,y,z,c);
                    pValCircTot += pValCirc;
                    runs++;
                }
                if (runs==0) check=0; //should only happen if entire segment is returning indefinite coords
                else check=pValCircTot/runs;
            } while (check > pVal*2/3);
        }
        radTot += rad;
        radArr.push_back(rad);
    }

    radAve = radTot/(segment.size()-2);
    double stR,t;
    for (int i=0; i<radArr.size(); i++)
    {
        t = (radArr[i]-radAve)*(radArr[i]-radAve);
        stR += t;
    }
    outputRadStDev = sqrt(stR/radArr.size());
    outputRadAve=radAve;
    return true;

}

//write and save QList NeuronTree into file
bool export_list2file(QList<NeuronTree> & N2, QString fileSaveName, QString fileOpenName)
{
    QFile file(fileSaveName);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text))
        return false;
    QTextStream myFile(&file);
    myFile<<"# generated by Vaa3D Plugin auto_identify - label neurons"<<endl;
    myFile<<"# source file(s): "<<fileOpenName<<endl;
    myFile<<"# id,type,x,y,z,r,pid"<<endl;
    QList<NeuronSWC> N1;
    for (int j=0; j<N2.count(); j++)
    {
        N1 = N2.at(j).listNeuron;
        for (V3DLONG i=0;i<N1.size();i++)
        {
            myFile << N1.at(i).n <<" " << N1.at(i).type << " "<< N1.at(i).x <<" "<<N1.at(i).y << " "<< N1.at(i).z << " "<< N1.at(i).r << " " <<N1.at(i).pn << "\n";
        }
    }
    file.close();
    v3d_msg(QString("SWC file %1 has been generated, size %2").arg(fileSaveName).arg(N1.size()));
    cout<<"swc file "<<fileSaveName.toStdString()<<" has been generated, size: "<<N1.size()<<endl;
    return true;
}

template <class T> bool apply_mask(unsigned char* data1d, V3DLONG *dimNum,
                                   int xc, int yc, int zc, int c, double threshold, T & maskImg)
{
    V3DLONG N = dimNum[0];
    V3DLONG M = dimNum[1];
    V3DLONG P = dimNum[2];
    int x,y,z;
    double pi=3.14,rad,radZ,dataAve;

    compute_cell_values_rad(data1d,dimNum,xc,yc,zc,c,threshold,dataAve,rad,radZ);
    *maskImg.at(xc,yc,zc,0) = 255;
    double angle = pi/2;

    for (double r=0.5; r<=rad; r+=0.5)
    {
        for (double theta=0; theta<2*pi; theta+=angle)
        {
            for (double phi=0; phi<pi; phi+=angle)
            {
                x = xc+r*cos(theta)*sin(phi);
                if (x>N-1) x=N-1; if (x<0) x=0;

                y = yc+r*sin(theta)*sin(phi);
                if (y>M-1) y=M-1; if (y<0) y=0;
                z = zc+r*cos(phi);
                if (z>P-1) z=P-1; if (z<0) z=0;
                *maskImg.at(x,y,z,0) = 255;
            }
        }
        if (angle>=pi/16 && r>=1) angle/=2;
    }
    return true;
}

bool open_testSWC(QString &fileOpenName, NeuronTree & openTree)
{
    if (!fileOpenName.isEmpty() && fileOpenName.toUpper().endsWith(".SWC"))
    {
        openTree = readSWC_file(fileOpenName);
        return true;
    }
    else
    {
        v3d_msg("You did not choose a valid file type, or the file you chose is empty. Will attempt to find exampler set in window.");
        return false;
    }
}


//from movieConverter plugin
QString getAppPath()
{
    QDir testPluginsDir = QDir(qApp->applicationDirPath());

#if defined(Q_OS_WIN)
    if (testPluginsDir.dirName().toLower() == "debug" || testPluginsDir.dirName().toLower() == "release")
        testPluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    // In a Mac app bundle, plugins directory could be either
    //  a - below the actual executable i.e. v3d.app/Contents/MacOS/plugins/
    //  b - parallel to v3d.app i.e. foo/v3d.app and foo/plugins/
    if (testPluginsDir.dirName() == "MacOS") {
        QDir testUpperPluginsDir = testPluginsDir;
        testUpperPluginsDir.cdUp();
        testUpperPluginsDir.cdUp();
        testUpperPluginsDir.cdUp(); // like foo/plugins next to foo/v3d.app
        if (testUpperPluginsDir.cd("plugins")) testPluginsDir = testUpperPluginsDir;
        testPluginsDir.cdUp();
    }
#endif

    return testPluginsDir.absolutePath();
}


//from regiongrow plugin
bool regiongrowing(V3DPluginCallback2 &callback, int c, double thresh, double rad,
                   LandmarkList &cmList, vector<int> &volList,
                   unsigned char* &outimg8, unsigned short* &outimg16, float* &outimg32)
{
    Image4DSimple* subject = callback.getImage(callback.currentImageWindow());
    if (!subject)
        return false;
    ROIList pRoiList = callback.getROI(callback.currentImageWindow());

    V3DLONG ch_rgb = c;
    V3DLONG th_idx = 5;
    double threshold = thresh;

    V3DLONG volsz = 50;
    if (pow(rad,3.0)<50) volsz=pow(rad,3.0);

    ImagePixelType datatype_subject = subject->getDatatype();

    if(datatype_subject != V3D_UINT8)
    {
        v3d_msg("Currently this program only support 8-bit data.");
        return false;
    }

    unsigned char* pSub = subject->getRawData();

    V3DLONG sz0 = subject->getXDim();
    V3DLONG sz1 = subject->getYDim();
    V3DLONG sz2 = subject->getZDim();
    V3DLONG sz3 = subject->getCDim();

    V3DLONG pagesz_sub = sz0*sz1*sz2;
    V3DLONG offset_sub = (ch_rgb-1)*pagesz_sub;

    //---------------------------------------------------------------------------------------------------------------------------------------------------
    //finding the bounding box of ROI
    bool vxy=true,vyz=true,vzx=true; // 3 2d-views

    QRect b_xy = pRoiList.at(0).boundingRect();
    QRect b_yz = pRoiList.at(1).boundingRect();
    QRect b_zx = pRoiList.at(2).boundingRect();

    if(b_xy.left()==-1 || b_xy.top()==-1 || b_xy.right()==-1 || b_xy.bottom()==-1)
        vxy=false;
    if(b_yz.left()==-1 || b_yz.top()==-1 || b_yz.right()==-1 || b_yz.bottom()==-1)
        vyz=false;
    if(b_zx.left()==-1 || b_zx.top()==-1 || b_zx.right()==-1 || b_zx.bottom()==-1)
        vzx=false;

    V3DLONG bpos_x, bpos_y, bpos_z, bpos_c, epos_x, epos_y, epos_z, epos_c;

    // 8 cases
    if(vxy && vyz && vzx) // all 3 2d-views
    {
        bpos_x = qBound(V3DLONG(0), V3DLONG(qMax(b_xy.left(), b_zx.left())), sz0-1);
        bpos_y = qBound(V3DLONG(0), V3DLONG(qMax(b_xy.top(),  b_yz.top())), sz1-1);
        bpos_z = qBound(V3DLONG(0), V3DLONG(qMax(b_yz.left(), b_zx.top())), sz2-1);

        epos_x = qBound(V3DLONG(0), V3DLONG(qMin(b_xy.right(), b_zx.right())), sz0-1);
        epos_y = qBound(V3DLONG(0), V3DLONG(qMin(b_xy.bottom(), b_yz.bottom())), sz1-1);
        epos_z = qBound(V3DLONG(0), V3DLONG(qMin(b_yz.right(), b_zx.bottom())), sz2-1);
    }
    else if(!vxy && vyz && vzx) // 2 of 3
    {
        bpos_x = qBound(V3DLONG(0), V3DLONG(qMax(0, b_zx.left())), sz0-1);
        bpos_y = qBound(V3DLONG(0), V3DLONG(qMax(0,  b_yz.top())), sz1-1);
        bpos_z = qBound(V3DLONG(0), V3DLONG(qMax(b_yz.left(), b_zx.top())), sz2-1);

        epos_x = qBound(V3DLONG(0), V3DLONG(qMin((V3DLONG)(sz0-1), (V3DLONG)b_zx.right())), sz0-1);
        epos_y = qBound(V3DLONG(0), V3DLONG(qMin((V3DLONG)(sz1-1), (V3DLONG)b_yz.bottom())), sz1-1);
        epos_z = qBound(V3DLONG(0), V3DLONG(qMin(b_yz.right(), b_zx.bottom())), sz2-1);
    }
    else if(vxy && !vyz && vzx)
    {
        bpos_x = qBound(V3DLONG(0), V3DLONG(qMax(b_xy.left(), b_zx.left())), sz0-1);
        bpos_y = qBound(V3DLONG(0), V3DLONG(qMax(b_xy.top(),  0)), sz1-1);
        bpos_z = qBound(V3DLONG(0), V3DLONG(qMax(0, b_zx.top())), sz2-1);

        epos_x = qBound(V3DLONG(0), V3DLONG(qMin(b_xy.right(), b_zx.right())), sz0-1);
        epos_y = qBound(V3DLONG(0), V3DLONG(qMin((V3DLONG)b_xy.bottom(), (V3DLONG)(sz1-1))), sz1-1);
        epos_z = qBound(V3DLONG(0), V3DLONG(qMin((V3DLONG)(sz2-1), (V3DLONG)b_zx.bottom())), sz2-1);
    }
    else if(vxy && vyz && !vzx)
    {
        bpos_x = qBound(V3DLONG(0), V3DLONG(qMax(b_xy.left(), 0)), sz0-1);
        bpos_y = qBound(V3DLONG(0), V3DLONG(qMax(b_xy.top(),  b_yz.top())), sz1-1);
        bpos_z = qBound(V3DLONG(0), V3DLONG(qMax(b_yz.left(), 0)), sz2-1);

        epos_x = qBound(V3DLONG(0), V3DLONG(qMin((V3DLONG)b_xy.right(), (V3DLONG)(sz0-1))), sz0-1);
        epos_y = qBound(V3DLONG(0), V3DLONG(qMin(b_xy.bottom(), b_yz.bottom())), sz1-1);
        epos_z = qBound(V3DLONG(0), V3DLONG(qMin((V3DLONG)b_yz.right(), (V3DLONG)(sz2-1))), sz2-1);
    }
    else if(vxy && !vyz && !vzx) // only 1 of 3
    {
        bpos_x = qBound(V3DLONG(0), V3DLONG(qMax(b_xy.left(), 0)), sz0-1);
        bpos_y = qBound(V3DLONG(0), V3DLONG(qMax(b_xy.top(),  0)), sz1-1);
        bpos_z = 0;

        epos_x = qBound(V3DLONG(0), V3DLONG(qMin((V3DLONG)b_xy.right(), (V3DLONG)(sz0-1))), sz0-1);
        epos_y = qBound(V3DLONG(0), V3DLONG(qMin((V3DLONG)b_xy.bottom(), (V3DLONG)(sz1-1))), sz1-1);
        epos_z = sz2-1;
    }
    else if(!vxy && vyz && !vzx)
    {
        bpos_x = 0;
        bpos_y = qBound(V3DLONG(0), V3DLONG(qMax(0,  b_yz.top())), sz1-1);
        bpos_z = qBound(V3DLONG(0), V3DLONG(qMax(b_yz.left(), 0)), sz2-1);

        epos_x = sz0-1;
        epos_y = qBound(V3DLONG(0), V3DLONG(qMin((V3DLONG)(sz1-1), (V3DLONG)b_yz.bottom())), sz1-1);
        epos_z = qBound(V3DLONG(0), V3DLONG(qMin((V3DLONG)b_yz.right(), (V3DLONG)(sz2-1))), sz2-1);
    }
    else if(!vxy && !vyz && vzx)
    {
        bpos_x = qBound(V3DLONG(0), V3DLONG(qMax(0, b_zx.left())), sz0-1);
        bpos_y = 0;
        bpos_z = qBound(V3DLONG(0), V3DLONG(qMax(0, b_zx.top())), sz2-1);

        epos_x = qBound(V3DLONG(0), V3DLONG(qMin((V3DLONG)(sz0-1), (V3DLONG)b_zx.right())), sz0-1);
        epos_y = sz1-1;
        epos_z = qBound(V3DLONG(0), V3DLONG(qMin((V3DLONG)(sz2-1), (V3DLONG)b_zx.bottom())), sz2-1);
    }
    else // 0
    {
        bpos_x = 0;
        bpos_y = 0;
        bpos_z = 0;

        epos_x = sz0-1;
        epos_y = sz1-1;
        epos_z = sz2-1;
    }

    //qDebug("x %d y %d z %d x %d y %d z %d ",bpos_x,bpos_y,bpos_z,epos_x,epos_y,epos_z);

    //ROI extraction
    V3DLONG sx = (epos_x-bpos_x)+1;
    V3DLONG sy = (epos_y-bpos_y)+1;
    V3DLONG sz = (epos_z-bpos_z)+1;
    V3DLONG sc = sz3; // 0,1,2

    //choose the channel stack
    V3DLONG pagesz = sx*sy*sz;

    double meanv=0, stdv=0;

    //------------------------------------------------------------------------------------------------------------------------------------

    unsigned char *data1d = NULL;

    try
    {
        data1d = new unsigned char [pagesz];

        unsigned char *pSubject = (unsigned char *)pSub + offset_sub;

        for(V3DLONG k=bpos_z; k<=epos_z; k++)
        {
            V3DLONG offset_z = k*sz0*sz1;
            V3DLONG offset_crop_z = (k-bpos_z)*sx*sy;
            for(V3DLONG j=bpos_y; j<=epos_y; j++)
            {
                V3DLONG offset_y = j*sz0 + offset_z;
                V3DLONG offset_crop_y = (j-bpos_y)*sx + offset_crop_z;
                for(V3DLONG i=bpos_x; i<=epos_x; i++)
                {
                    data1d[(i-bpos_x) + offset_crop_y] = pSubject[i+offset_y];

                    meanv += data1d[(i-bpos_x) + offset_crop_y];
                }
            }
        }

    }
    catch(...)
    {
        printf("Fail to allocate memory.\n");
        return false;
    }


    meanv /= pagesz;

    for(V3DLONG i=0; i<pagesz; i++)
        stdv += ((double)data1d[i] - meanv)*((double)data1d[i] - meanv);

    stdv /= (pagesz-1);
    stdv = sqrt(stdv);

    //qDebug() << "meanv ..." << meanv << "stdv ..." << stdv;

    //----------------------------------------------------------------------------------------------------------------------------------
    // bw
    unsigned char *bw = new unsigned char [pagesz];

    if(th_idx == 0)
    {
        threshold = meanv;
    }
    else if(th_idx == 1)
    {
        threshold = meanv + stdv;
    }

    for(V3DLONG i=0; i<pagesz; i++)
        bw[i] = ((double)data1d[i]>threshold)?255:0;


    //
    V3DLONG offset_y, offset_z;

    offset_y=sx;
    offset_z=sx*sy;

    V3DLONG neighbors = 26;
    V3DLONG neighborhood_26[26] = {-1, 1, -offset_y, offset_y, -offset_z, offset_z,
        -offset_y-1, -offset_y+1, -offset_y-offset_z, -offset_y+offset_z,
        offset_y-1, offset_y+1, offset_y-offset_z, offset_y+offset_z,
        offset_z-1, offset_z+1, -offset_z-1, -offset_z+1,
        -1-offset_y-offset_z, -1-offset_y+offset_z, -1+offset_y-offset_z, -1+offset_y+offset_z,
        1-offset_y-offset_z, 1-offset_y+offset_z, 1+offset_y-offset_z, 1+offset_y+offset_z};

    // eliminate volume with only one single voxel
    for(V3DLONG k = 0; k < sz; k++)
    {
        V3DLONG idxk = k*offset_z;
        for(V3DLONG j = 0;  j < sy; j++)
        {
            V3DLONG idxj = idxk + j*offset_y;
            for(V3DLONG i = 0, idx = idxj; i < sx;  i++, idx++)
            {
                if(i==0 || i==sx-1 || j==0 || j==sy-1 || k==0 || k==sz-1)
                    continue;

                if(bw[idx])
                {
                    bool one_point = true;
                    for(int ineighbor=0; ineighbor<neighbors; ineighbor++)
                    {
                        V3DLONG n_idx = idx + neighborhood_26[ineighbor];

                        if(bw[n_idx])
                        {
                            one_point = false;
                            break;
                        }

                    }
                    if(one_point==true)
                        bw[idx] = 0;
                }

            }
        }
    }

//    // display BW image
//    Image4DSimple p4Dbw;
//    p4Dbw.setData((unsigned char*)bw, sx, sy, sz, 1, V3D_UINT8);

//    v3dhandle newwinbw = callback.newImageWindow();
//    callback.setImage(newwinbw, &p4Dbw);
//    callback.setImageName(newwinbw, "black-white image after thresholding");
//    callback.updateImageWindow(newwinbw);

    // 3D region growing
    //----------------------------------------------------------------------------------------------------------------------------------

    //declaration
    V3DLONG totalpxlnum=pagesz;

    RgnGrow3dClass * pRgnGrow = new RgnGrow3dClass;
    if (!pRgnGrow)
    {
        printf("Fail to allocate memory for RgnGrow3dClass().");
    }

    pRgnGrow->ImgDep = sz;
    pRgnGrow->ImgHei = sy;
    pRgnGrow->ImgWid = sx;

    if (newIntImage3dPairMatlabProtocol(pRgnGrow->quantImg3d,pRgnGrow->quantImg1d,pRgnGrow->ImgDep,pRgnGrow->ImgHei,pRgnGrow->ImgWid)==0) return false;

    int nstate;
    UBYTE minlevel,maxlevel;

    copyvecdata((unsigned char *)bw,totalpxlnum,pRgnGrow->quantImg1d,nstate,minlevel,maxlevel);

    //don't count lowest level (background here = 0)
    minlevel = minlevel+1;
    if (minlevel>maxlevel)
        minlevel = maxlevel; //at least do one level

    //begin computation
    phcDebugPosNum = 0;
    phcDebugRgnNum = 0;

    if (newIntImage3dPairMatlabProtocol(pRgnGrow->PHCLABELSTACK3d,pRgnGrow->PHCLABELSTACK1d,1,3,totalpxlnum)==0) return false;
    pRgnGrow->PHCLABELSTACKPOS = 0;

    pRgnGrow->PHCURGN = new RGN;
    if (!pRgnGrow->PHCURGN)
    {
        printf("Unable to do:pRgnGrow->PHCURGN = new RGN;  -->current phcDebugRgnNum=%i.\n",phcDebugRgnNum);
    }
    pRgnGrow->PHCURGN_head = pRgnGrow->PHCURGN;
    pRgnGrow->TOTALRGNnum = 1;

    deleteIntImage3dPairMatlabProtocol(pRgnGrow->PHCDONEIMG3d,pRgnGrow->PHCDONEIMG1d);
    if (newIntImage3dPairMatlabProtocol(pRgnGrow->PHCDONEIMG3d,pRgnGrow->PHCDONEIMG1d,pRgnGrow->ImgDep,pRgnGrow->ImgHei,pRgnGrow->ImgWid)==0) return false;

    for(int j=minlevel;j<=maxlevel;j++)
    {
        int depk, colj, rowi;

        BYTE * PHCDONEIMG1d = pRgnGrow->PHCDONEIMG1d;
        UBYTE * quantImg1d = pRgnGrow->quantImg1d;
        BYTE *** flagImg = pRgnGrow->PHCDONEIMG3d;
        for (V3DLONG tmpi=0; tmpi<totalpxlnum; tmpi++)
        {
            PHCDONEIMG1d[tmpi] = (quantImg1d[tmpi]==(UBYTE)j)?1:0;
        }

        pRgnGrow->PHCURLABEL = 0;

        for(depk=0; depk<pRgnGrow->ImgDep; depk++)
            for(colj=0; colj<pRgnGrow->ImgHei; colj++)
                for(rowi=0; rowi<pRgnGrow->ImgWid; rowi++)
                {
                    if (flagImg[depk][colj][rowi]==1)
                    {
                        pRgnGrow->IFINCREASELABEL = 1;

                        pRgnGrow->PHCURLABEL++;

                        pRgnGrow->PHCLABELSTACKPOS = 0;

                        pRgnGrow->PHCLABELSTACK3d[0][0][pRgnGrow->PHCLABELSTACKPOS] = depk;
                        pRgnGrow->PHCLABELSTACK3d[0][1][pRgnGrow->PHCLABELSTACKPOS] = colj;
                        pRgnGrow->PHCLABELSTACK3d[0][2][pRgnGrow->PHCLABELSTACKPOS] = rowi;

                        //create pos memory
                        pRgnGrow->PHCURGNPOS = new POS;
                        if (pRgnGrow->PHCURGNPOS==0)
                        {
                            printf("Fail to allocate memory! -->cur phcDebugPosNum=%i.\n",phcDebugPosNum);
                        }
                        pRgnGrow->PHCURGNPOS_head = pRgnGrow->PHCURGNPOS;
                        pRgnGrow->TOTALPOSnum = 1;

                        while(1)
                        {
                            pRgnGrow->IFINCREASELABEL = 1;
                            V3DLONG posbeg = pRgnGrow->PHCLABELSTACKPOS;
                            V3DLONG mypos = posbeg;

                            while (mypos>=0)
                            {
                                pRgnGrow->STACKCNT = 0;
                                int curdep = pRgnGrow->PHCLABELSTACK3d[0][0][mypos];
                                int curcol = pRgnGrow->PHCLABELSTACK3d[0][1][mypos];
                                int currow = pRgnGrow->PHCLABELSTACK3d[0][2][mypos];

                                if (flagImg[curdep][curcol][currow]==1)
                                {
                                    rgnfindsub(currow,curcol,curdep,0,1,pRgnGrow);
                                }
                                else if(flagImg[curdep][curcol][currow]==-1)
                                {
                                    rgnfindsub(currow,curcol,curdep,0,0,pRgnGrow);
                                }

                                V3DLONG posend = pRgnGrow->PHCLABELSTACKPOS;

                                if (posend>posbeg)
                                {mypos = pRgnGrow->PHCLABELSTACKPOS;}
                                else
                                {mypos = mypos-1;}
                                posbeg = posend;
                            }

                            if (pRgnGrow->IFINCREASELABEL==1)
                                break;
                        }

                        //set pos as member of current RGN
                        pRgnGrow->PHCURGN->layer = j;
                        pRgnGrow->PHCURGN->no = pRgnGrow->PHCURLABEL;
                        pRgnGrow->PHCURGN->poslist = pRgnGrow->PHCURGNPOS_head;
                        pRgnGrow->PHCURGN->poslistlen = pRgnGrow->TOTALPOSnum;

                        pRgnGrow->TOTALPOSnum = 0;

                        pRgnGrow->PHCURGN->next = new RGN;
                        if(pRgnGrow->PHCURGN->next==0)
                        {printf("fail to do --> pRgnGrow->PHCURGN->next = new RGN;\n");}
                        pRgnGrow->PHCURGN = pRgnGrow->PHCURGN->next;
                        pRgnGrow->TOTALRGNnum++;
                    }
                }
    }
    //de-alloc
    if(data1d) {delete []data1d; data1d = NULL;}

    //
    //int end_t = clock();
    //qDebug() << "label objects via region growing time elapse ..." << end_t-start_t;

    //find the second big area in labeling
    STCL *staRegion = new STCL;
    STCL *staRegion_begin = staRegion;
    RGN *curRgn = pRgnGrow->PHCURGN_head;
    V3DLONG nrgncopied = 0; //num of rgn output

    std::vector<STCL> stclList;

    while(curRgn && curRgn->next)
    {
        //qDebug() << "num of rgn ..." << nrgncopied << curRgn->no << curRgn->next->no;

        staRegion->no = curRgn->no;
        staRegion->count = 0;

        POS * curPos = curRgn->poslist;

        V3DLONG count = 0;
        staRegion->desposlist = new V3DLONG [curRgn->poslistlen-1];
        while(curPos && curPos->next)
        {
            staRegion->desposlist[count++] = curPos->pos;
            curPos = curPos->next;
        }
        staRegion->count = count;

        //qDebug() << "pixels ..." << count;

        if(count<volsz) { nrgncopied++; curRgn = curRgn->next; continue; } // filter out the small components

        //
        stclList.push_back(*staRegion);

        //
        curRgn = curRgn->next;
        staRegion->next = new STCL;
        staRegion = staRegion->next;

        nrgncopied++;

    }

    // result pointer
    V3DLONG length;

    V3DLONG n_rgn = stclList.size(); // qMin(5, nrgncopied);

    //qDebug() << "display "<< n_rgn<<" rgns from "<< nrgncopied;

    Image4DSimple regionMap;
    if(n_rgn>const_int_floatCriteria)
    {
        float *pRGCL = NULL;
        try
        {
            pRGCL = new float [pagesz];

            memset(pRGCL, 0, sizeof(float)*pagesz);
        }
        catch (...)
        {
            printf("Fail to allocate memory.\n");
            return false;
        }

        for(int ii=0; ii<n_rgn; ii++)
        {
            length = stclList.at(ii).count; //a[ii];

            //qDebug() << "region ..." << ii << length;

            // find idx
            V3DLONG *cutposlist = stclList.at(ii).desposlist;

            float scx=0,scy=0,scz=0,si=0;

            for(int i=0; i<length; i++)
            {
                //qDebug() << "idx ..." << i << cutposlist[i] << pagesz;

                pRGCL[ cutposlist[i] ] = (float)ii + 1.0;

                float cv = pSub[ cutposlist[i] ];

                V3DLONG idx = cutposlist[i];

                V3DLONG k1 = idx/(sx*sy);
                V3DLONG j1 = (idx - k1*sx*sy)/sx;
                V3DLONG i1 = idx - k1*sx*sy - j1*sx;

                scz += k1*cv;
                scy += j1*cv;
                scx += i1*cv;
                si += cv;
            }

            if (si>0)
            {
                V3DLONG ncx = scx/si + 0.5 +1;
                V3DLONG ncy = scy/si + 0.5 +1;
                V3DLONG ncz = scz/si + 0.5 +1;

                LocationSimple pp(ncx, ncy, ncz);
                cmList.push_back(pp);
                int vol = (int)length;
                volList.push_back(vol);

            }

        }
        //regionMap.setData((unsigned char*)pRGCL, sx, sy, sz, 1, V3D_FLOAT32);
        outimg32 = (float*)pRGCL;
    }
    else if(n_rgn>const_int_longCriteria)
    {
        unsigned short *pRGCL = NULL;
        try
        {
            pRGCL = new unsigned short [pagesz];

            memset(pRGCL, 0, sizeof(unsigned short)*pagesz);
        }
        catch (...)
        {
            printf("Fail to allocate memory.\n");
            return false;
        }

        for(int ii=0; ii<n_rgn; ii++)
        {
            length = stclList.at(ii).count; //a[ii];

            //qDebug() << "region ..." << ii << length;

            // find idx
            V3DLONG *cutposlist = stclList.at(ii).desposlist;

            float scx=0,scy=0,scz=0,si=0;

            for(int i=0; i<length; i++)
            {
                //qDebug() << "idx ..." << i << cutposlist[i] << pagesz;

                pRGCL[ cutposlist[i] ] = (unsigned short)ii + 1;

                float cv = pSub[ cutposlist[i] ];

                V3DLONG idx = cutposlist[i];

                V3DLONG k1 = idx/(sx*sy);
                V3DLONG j1 = (idx - k1*sx*sy)/sx;
                V3DLONG i1 = idx - k1*sx*sy - j1*sx;

                scz += k1*cv;
                scy += j1*cv;
                scx += i1*cv;
                si += cv;
            }

            if (si>0)
            {
                V3DLONG ncx = scx/si + 0.5 +1;
                V3DLONG ncy = scy/si + 0.5 +1;
                V3DLONG ncz = scz/si + 0.5 +1;

                LocationSimple pp(ncx, ncy, ncz);
                cmList.push_back(pp);
                int vol = (int)length;
                volList.push_back(vol);

            }

        }
        //regionMap.setData((unsigned char*)pRGCL, sx, sy, sz, 1, V3D_UINT16);

        outimg16 = (unsigned short*)pRGCL;
    }
    else
    {
        unsigned char *pRGCL = NULL;
        try
        {
            pRGCL = new unsigned char [pagesz];

            memset(pRGCL, 0, pagesz);
        }
        catch (...)
        {
            printf("Fail to allocate memory.\n");
            return false;
        }

        for(int ii=0; ii<n_rgn; ii++)
        {
            length = stclList.at(ii).count; //a[ii];

            //qDebug() << "region ..." << ii << length;

            // find idx
            V3DLONG *cutposlist = stclList.at(ii).desposlist;

            float scx=0,scy=0,scz=0,si=0;

            for(int i=0; i<length; i++)
            {
                //qDebug() << "idx ..." << i << cutposlist[i] << pagesz;

                pRGCL[ cutposlist[i] ] = (unsigned char)ii + 1;

                float cv = pSub[ cutposlist[i] ];

                V3DLONG idx = cutposlist[i];

                V3DLONG k1 = idx/(sx*sy);
                V3DLONG j1 = (idx - k1*sx*sy)/sx;
                V3DLONG i1 = idx - k1*sx*sy - j1*sx;

                scz += k1*cv;
                scy += j1*cv;
                scx += i1*cv;
                si += cv;
            }

            if (si>0)
            {
                V3DLONG ncx = scx/si + 0.5 +1;
                V3DLONG ncy = scy/si + 0.5 +1;
                V3DLONG ncz = scz/si + 0.5 +1;

                LocationSimple pp(ncx, ncy, ncz);
                cmList.push_back(pp);
                int vol = (int)length;
                volList.push_back(vol);

            }

        }
        //regionMap.setData((unsigned char*)pRGCL, sx, sy, sz, 1, V3D_UINT8);
        outimg8 = (unsigned char*)pRGCL;
    }
    //de-alloc
    deleteIntImage3dPairMatlabProtocol(pRgnGrow->quantImg3d,pRgnGrow->quantImg1d);
    deleteIntImage3dPairMatlabProtocol(pRgnGrow->PHCLABELSTACK3d,pRgnGrow->PHCLABELSTACK1d);
    deleteIntImage3dPairMatlabProtocol(pRgnGrow->PHCDONEIMG3d,pRgnGrow->PHCDONEIMG1d);
    cout<<"success, region count "<<cmList.count()<<endl;
    return true;

}

template <class T> bool segment_regions(unsigned char* data1d, V3DLONG *dimNum,

                                        LandmarkList inputList,
                                        LandmarkList &newList,
                                        LandmarkList regionList,
                                        vector<int> regionVol,
                                        double radAve, double radStDev, double radZ,
                                        int c, int cat, double thresh,
                                        T* regionData)
{
    V3DLONG N = dimNum[0];
    V3DLONG M = dimNum[1];
    V3DLONG P = dimNum[2];

	//ofstream logFile;
	//logFile.open ("log_segment_regions.txt");

    //set up mask
    unsigned char *maskData = 0;
    maskData = new unsigned char [N*M*P];
    for (V3DLONG tmpi=0;tmpi<N*M*P;++tmpi) maskData[tmpi] = 0; //preset to be all 0
    
    //set regions with 1 cell into newList and mask
    int n_rgn = regionList.size();
    vector<int> rgn_cellcount;
    double VolAve,VolMax,VolMin;
    if (radZ<=0) //if radius was calculated spherically
    {
        VolAve = (4/3)*PI*pow(radAve,3.0);
        VolMax = (4/3)*PI*pow((radAve+radStDev),3.0);        
        VolMin = (4/3)*PI*pow((radAve-radStDev),3.0);
    }
    else //if radius was calculated with 2D circle
    {
        VolAve = (4/3)*PI*pow(radAve,2.0)*radZ;
        VolMax = (4/3)*PI*pow((radAve+radStDev),2.0)*radZ;
        VolMin = (4/3)*PI*pow((radAve-radStDev),2.0)*radZ;
    }
    for (int i=0; i<n_rgn; i++)
    {
        double vol = regionVol.at(i)/VolAve + 0.5;
        double vol2 = regionVol.at(i)/VolMax + 0.5;
        double vol3 = regionVol.at(i)/VolMin;
        int volcheck = (int) vol;
        int volcheck2 = (int) vol2;
        LocationSimple tmp(0,0,0);
        int xc,yc,zc;
        tmp = regionList.at(i);
        tmp.getCoord(xc,yc,zc);
        if (vol3<1 || volcheck2>5) //if region is too small or too big, compare with input marker list to see if it is cell cluster or noise
        {
            rgn_cellcount.push_back(-5);
        }
        else if (volcheck<=1)
        {
            newList.append(regionList.at(i));
            rgn_cellcount.push_back(0);
        }
        else rgn_cellcount.push_back(volcheck);
    }
    //logFile<<"Filter regions by size succeed!!!"<<endl;

    //add inputList into newList
    int rad_z = radZ;
    if (rad_z<=0) rad_z = (int)radAve;
    for (int i=0; i<inputList.count(); i++)
    {
        LocationSimple tmp(0,0,0);
        int xc,yc,zc;
        tmp = inputList.at(i);
        tmp.getCoord(xc,yc,zc);
        int regNum = regionData[zc*M*N+yc*N+xc];
        if (regNum<=0) continue;
        //if this region only has one cell, it will have already been accounted for
        if (rgn_cellcount.at(regNum-1)==0) continue;
        if (rgn_cellcount.at(regNum-1)==-5) rgn_cellcount.at(regNum-1)= (int)(regionVol.at(regNum-1)/VolAve + 0.5);
        //if this region has multiple cells, add input marker to newList, mask around input marker, update rgn_cellcount
        newList.append(tmp);
        for (int x=xc-radAve; x<xc+radAve; x++)
        {
            for (int y=yc-radAve; y<yc+radAve; y++)
            {
                for (int z=zc-rad_z; z<zc+rad_z; z++)
                {
                    if (x<1 || x>N || y<1 || y>M || z<1 || z>P) continue;
                    if (regionData[(z-1)*M*N+(y-1)*N+(x-1)]==regNum)
                        maskData[(z-1)*M*N+(y-1)*N+(x-1)]=const_double_maxPixelIntensity;
                }
            }
        }
        rgn_cellcount.at(regNum-1) -= 1;
    }
    //logFile<<"Mask construction succeed!!!"<<endl;

    //create cell template
    double t = (int)radAve*2+1;
    int length;
    if (radZ<=0) length = pow(t,3);
    else length = pow(t,2)*((int)radZ*2+1);
    vector<int> cell_template(length,0);
    int template_count=0;
    for (int i=0; i<inputList.count(); i++)
    {
        LocationSimple tmp = inputList.at(i);
        int xc,yc,zc,ind=-1;
        tmp.getCoord(xc,yc,zc);
        if (xc-radAve<1 || xc+radAve>N || yc-radAve<1 || yc+radAve>M || zc-radZ<1 || zc+radZ>P) continue;
        for (int x=xc-(int)radAve; x<=xc+(int)radAve; x++)
        {
            for (int y=yc-(int)radAve; y<=yc+(int)radAve; y++)
            {
                for (int z=zc-(int)rad_z; z<=zc+(int)rad_z; z++)
                {
                    ind++;
                    cell_template[ind]+=data1d[(c-1)*P*M*N+(z-1)*M*N+(y-1)*N+(x-1)];
                }
            }
        }
        template_count++;
    }

    double Xmean=0, Sx=0;
    if (template_count>0)
    {
        for (int i=0; i<length; i++)
        {
			if (template_count < 1)
			{
				v3d_msg("Warning: no templates created for cell identification!!!");
				cell_template[i] = 0;
			}
			else
			{
				cell_template[i] /= template_count;
			}
            Xmean+=cell_template[i];
        }
        Xmean /= length;
        for (int i=0; i<length; i++)
        {
            Sx+=pow(cell_template[i]-Xmean,2.0);
        }
        Sx = pow(Sx,0.5);
    }
	//logFile<<"Cell template construction succeed!!!"<<endl;

    //count cells in remaining regions
	//logFile<<"Segmenting large clusters start, totally "<<n_rgn<<" clusters!!!"<<endl;
	//logFile<<"rgn_cellcount has totally "<<rgn_cellcount.size()<<" cells!!!"<<endl;
	//logFile<<"regionList has totally "<<regionList.size()<<" regions!!!"<<endl;
    for (double i=0; i<n_rgn; i++) //region at i has value i+1
    {
		int count=rgn_cellcount.at(i);
        if (count<=0) {continue;}
        double range = 0;
		range =	pow(regionVol.at(i),0.33)*1.5;
		//logFile<<"range =	pow(regionVol.at(i),0.33)*1.5; succeed!!!"<<i<<endl;
		LocationSimple tmpLocation;
		tmpLocation = regionList.at(i);
		//logFile<<"tmpLocation = regionList.at(i); succeed!!!"<<i<<endl;
		//logFile<<"Segmentation start on region "<<i<<" !!!"<<endl;
		LandmarkList cells;
		cells = seg_by_mask (data1d,regionData,maskData,cell_template,dimNum,tmpLocation,radAve,radZ,range,count,i+1,c,thresh,Xmean,Sx);
		//logFile<<"cells = seg_by_mask (data1d,regionData,maskData,cell_template,dimNum,tmpLocation,radAve,radZ,range,count,i+1,c,thresh,Xmean,Sx); succeed!!!"<<endl;
		//logFile<<"Segmentation succeed on region "<<i<<" !!!"<<endl;
		for (int j=0; j<cells.size(); j++)
        {
            int x,y,z;
            LocationSimple tmpCell = cells.at(j);
            tmpCell.getCoord(x,y,z);
            tmpCell.x=x;
            tmpCell.y=y;
            tmpCell.z=z;
            tmpCell.category = cat;
            stringstream catStr;
            catStr << cat;
            tmpCell.comments = catStr.str();
            newList.append(tmpCell);
        }
		//logFile<<"Cells in region "<<i<<" added to the newList !!!"<<" Totally: "<<cells.size()<<"!!!"<<endl;
    }
	v3d_msg("Cell counting succeed!!!");
    return true;
}

template <class T> LandmarkList seg_by_mask (unsigned char* data1d, T* rgnData, unsigned char* maskData,
                                             vector<int> cell_template, V3DLONG *dimNum,
                                             LocationSimple cellLocation, int radAve, int radZ,
                                             double range, int cellcount, int rgn, int c, int thresh,
                                             double Xmean, double Sx)
{
	//ofstream logFile;
	//logFile.open ("log_seg_by_mask.txt");

    V3DLONG N = dimNum[0];
    V3DLONG M = dimNum[1];
    V3DLONG P = dimNum[2];
	V3DLONG total = N*M*P;

    if (radZ<=0) radZ=radAve;
    int xc,yc,zc;
    cellLocation.getCoord(xc,yc,zc);
    int vol = (4/3)*PI*pow(radAve,3.0);
    
    //define limits around region, would prefer to have eigenvalues to better determine range
    int xLow = xc-range; if (xLow<1) xLow=1;
    int xHigh = xc+range; if (xHigh>N) xHigh=N;
    int yLow = yc-range; if (yLow<1) yLow=1;
    int yHigh = yc+range; if (yHigh>M) yHigh=M;
    int zLow = zc-range; if (zLow<1) zLow=1;
    int zHigh = zc+range; if (zHigh>P) zHigh=P;
	int indexTmp = 0;
    LandmarkList outputList;
	//logFile <<"cellcount total: "<<cellcount<<"!!!"<<endl;
	for (int s=0; s<cellcount; s++)
    {
		//logFile <<"cell: "<<s<<" started!!!"<<endl;
		int min_vol=vol, min_diff=1000000;
        double corrMax=-1; //might want to define lower thresholds for rejection
        LocationSimple maxPos(0,0,0);
        for (int i=xLow; i<=xHigh; i++)
        {
			for (int j=yLow; j<=yHigh; j++)
            {
                for (int k=zLow; k<=zHigh; k++)
                {
					//logFile <<"if (maskData[(k-1)*M*N+(j-1)*N+(i-1)]!=0 || rgnData[(k-1)*M*N+(j-1)*N+(i-1)]!=rgn) hit!!! "<<i<<" "<<j<<" "<<k<<endl;
					if (maskData[(k-1)*M*N+(j-1)*N+(i-1)]!=0 || rgnData[(k-1)*M*N+(j-1)*N+(i-1)]!=rgn)
					{
						//logFile <<"if (maskData[(k-1)*M*N+(j-1)*N+(i-1)]!=0 || rgnData[(k-1)*M*N+(j-1)*N+(i-1)]!=rgn) succeed (true)!!! "<<i<<" "<<j<<" "<<k<<endl;
						continue;
					}
					else
					{
						//logFile <<"if (maskData[(k-1)*M*N+(j-1)*N+(i-1)]!=0 || rgnData[(k-1)*M*N+(j-1)*N+(i-1)]!=rgn) succeed (false)!!! "<<i<<" "<<j<<" "<<k<<endl;
					}
                    double t = radAve*2+1;
                    int length = pow(t,2)*(radZ*2+1);
                    vector<int> scoop_template(length,0);
                    int total_vol=0, template_diff=0, ind=-1;                    
                    double Ymean=0, Sy=0, PCC=0;
                    for (int x=i-radAve; x<=i+radAve; x++)
                    {
                        for (int y=j-radAve; y<=j+radAve; y++)
                        {
                            //These few lines are VERY likely to cause error of invalid acess to "scoop_template" or "rgnData";
							//If not handled carefully;
							//Current solution is temporary;
							//Updated by Xiang Li (lindbergh.li@gmail.com);
							for (int z=k-radZ; z<=k+radZ; z++)
                            {
                                ind++;
                                if (x<1 || x>N || y<1 || y>M || z<1 || z>P)
								{
									//logFile <<"scoop_template[ind]=0; hit!!! "<<i<<" "<<j<<" "<<k<<endl;
									scoop_template[ind]=0;
									//logFile <<"scoop_template[ind]=0; succeed!!! "<<i<<" "<<j<<" "<<k<<endl;
								}

								indexTmp = (z-1)*M*N+(y-1)*N+(x-1);
								if ((indexTmp>0)&&(indexTmp<total))
								{
									//logFile <<"if (maskData[indexTmp]!=0) hit!!! "<<indexTmp<<endl;
									if (maskData[indexTmp]!=0)
									{
										//logFile <<"if (maskData[indexTmp]!=0) succeed (true)!!! "<<indexTmp<<endl;
										goto loopcont;
									}
									else
									{
										//logFile <<"if (maskData[indexTmp]!=0) succeed (false)!!! "<<indexTmp<<endl;
									}
								}

								indexTmp = z*M*N+y*N+x;
								if ((indexTmp>0) && (indexTmp<total))
								{
									//logFile <<"if (rgnData[indexTmp]==rgn) hit!!! "<<indexTmp<<endl;
									if (rgnData[indexTmp]==rgn)
									{
										//logFile <<"if (rgnData[indexTmp]==rgn) succeed (true)!!! "<<indexTmp<<endl;
										total_vol++;
									}
									else
									{
										//logFile <<"if (rgnData[indexTmp]==rgn) succeed (false)!!! "<<indexTmp<<endl;
									}
								}
								
								
								int indexDanger = (c-1)*P*M*N+(z-1)*M*N+(y-1)*N+(x-1);
								if ((indexDanger > 0)&&(indexDanger<total))
								{
									//logFile <<"scoop_template[ind] = data1d[indexDanger]; hit!!! z: "<<z<<" indexDanger: "<<indexDanger<<" ind: "<<ind<<" k-radZ: "<<k-radZ<<" k+randZ: "<<k+radZ<<endl;
									scoop_template[ind] = data1d[indexDanger];
									//logFile <<"scoop_template[ind] = data1d[indexDanger]; succeed!!! z: "<<z<<" indexDanger: "<<indexDanger<<" ind: "<<ind<<" k-radZ: "<<k-radZ<<" k+randZ: "<<k+radZ<<endl;
								}
                            }
                        }
					}

					//logFile <<"Start correlation calculation!!! s: "<<s<<endl;
					for (int i=0; i<length; i++)
                    {
                        Ymean+=scoop_template[i];
                    }
                    Ymean /= length;
                    for (int i=0; i<length; i++)
                    {
                        Sy+=pow(scoop_template[i]-Ymean,2.0);
                    }
                    Sy = pow(Sy,0.5);
                    for (int i=0; i<length; i++)
                    {
						PCC += ((cell_template[i]-Xmean)*(scoop_template[i]-Ymean));
                    }
                    if (Sx*Sy==0) PCC=0;
                    else PCC /= (Sx*Sy);
                    if (abs(vol-total_vol)<min_vol && PCC>corrMax)
                    {
                        min_vol=abs(vol-total_vol);
                        corrMax=PCC;
                        maxPos.x=i; maxPos.y=j; maxPos.z=k;
                        if (corrMax==1) goto loopend;
                    }
					//logFile <<"Correlation calculation succeed!!! s: "<<s<<endl;
                    loopcont:;
                }
            }
        }
        loopend:;
        int xm,ym,zm;
        maxPos.getCoord(xm,ym,zm);
        if (xm==0 && ym==0 && zm==0) continue;
		mass_center_Coords(data1d,dimNum,xm,ym,zm,radAve,c,thresh);
		//logFile <<"mass_center_Coords(data1d,dimNum,xm,ym,zm,radAve,c,thresh); succeed!!! s: "<<s<<endl;
        double outAve,outRad,outZRad;
		compute_cell_values_rad(data1d,dimNum,xm,ym,zm,c,thresh,outAve,outRad,outZRad);
		//logFile <<"compute_cell_values_rad(data1d,dimNum,xm,ym,zm,c,thresh,outAve,outRad,outZRad); succeed!!! s: "<<s<<endl;
        if (outZRad<=0) outZRad=outRad;
        else outZRad*=1.5;
		if (maskData[(zm-1)*M*N+(ym-1)*N+(xm-1)]==0 && corrMax>0.3)
		{
			//logFile <<"if (maskData[(zm-1)*M*N+(ym-1)*N+(xm-1)]==0 && corrMax>0.3) hit and succeed!!! s: "<<s<<endl;
			maxPos.x=xm;maxPos.y=ym;maxPos.z=zm; outputList.append(maxPos);
			//logFile <<"maxPos.x=xm;maxPos.y=ym;maxPos.z=zm; outputList.append(maxPos); succeed!!! s: "<<s<<endl;
		} //update and append
		else
		{
			continue;
		}
		//logFile <<"Start filling maskData!!! s: "<<s<<endl;
        for (int x=xm-outRad; x<xm+outRad; x++)
        {
            for (int y=ym-outRad; y<ym+outRad; y++)
            {
                for (int z=zm-outZRad; z<zm+outZRad; z++)
                {
                    if (x<1 || x>N || y<1 || y>M || z<1 || z>P) continue;
					maskData[(z-1)*M*N+(y-1)*N+(x-1)]=255;
                }
            }
        }
		//logFile <<"Succeed filling maskData!!! s: "<<s<<endl;
        maxPos.x=0;maxPos.y=0;maxPos.z=0; //reset
	}
	//logFile <<"Doen with the region!!!"<<endl;
	//logFile <<rgn<<" "<<outputList.count()<<"!!!"<<endl;
    return outputList;
}
