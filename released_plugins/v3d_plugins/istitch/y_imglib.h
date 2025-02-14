/*
 #  y_imglib.h
 #
 #  created Feb. 12, 2010 by Yang Yu
 #  yuy@janelia.hhmi.org
 #
 */
//image and template class with funcs
//--------
#ifndef __Y_IMGLIB_H__
#define __Y_IMGLIB_H__
//--------
#include <complex>
#include <fftw3.h>
#include <cmath>
#include <ctime>
#include <cstring>
#include <fstream>
using std::ifstream;
#include <iostream>
using std::cerr;
using std::cout;
using std::endl;
#include <sstream>
#include <vector>
#include <list>
#include <bitset>
#include <set>
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/kruskal_min_spanning_tree.hpp"
#include "boost/graph/prim_minimum_spanning_tree.hpp"
// POSIX Threads
//#include <pthread.h> 
//#define PROCESSORS 1 // maximum threads
#define PI 3.14159265
#define INF 1E10
#define EPS 1E-10
#define NPEAKS 8 // numbers of peaks of phase correlation
#define NO_OBJECT 0 // for region growing
using namespace std;
// define different data types
typedef unsigned char UINT8;
//typedef double REAL; // -lfftw3 -lfftw3_threads // fftw_
typedef float REAL; // -lfftw3f -lfftw3f_threads // fftwf_
#define TC_COMMENT1 (" thumbnail file ")
#define TC_COMMENT2 (" tiles ")
#define TC_COMMENT3 (" dimensions (XYZC) ")
#define TC_COMMENT4 (" origin (XYZ) ")
#define TC_COMMENT5 (" resolution (XYZ) ")
#define TC_COMMENT6 (" image coordinates look up table ")
#define TC_COMMENT7 (" MST LUT ")
//statistics of count of labeling
class STCL
{
public:
    STCL(){}
    ~STCL(){}
public:
    int count;
    int label;
};
// this is an extension to 3D region growing from octave 2D bwlabel code
// find func copied from octave bwlabel code
static int find(int set[], int x)
{
    int r = x;
    while ( set[r] != r )
        r = set[r];
    return r;
}
template <class T>
T y_max(T x, T y)
{
    return (x>y)?x:y;
}
template <class T>
T y_min(T x, T y)
{
    return (x<y)?x:y;
}
template <class T>
T y_abs(T x)
{
    return (x<(T)0)?-x:x;
}
// delete 1d pointer
template <class T>
void y_del(T *&p)
{
    if(p) {delete []p; p=NULL;}
}
// new 1d pointer
template<class T, class Tidx>
void y_new(T *&p, Tidx N)
{
    //
    y_del<T>(p);
    
    //
    try
    {
        p = new T [N];
    }
    catch(...)
    {
        cout<<"Attempt to allocate memory failed!"<<endl;
        y_del<T>(p);
        return;
    }
}
template<class T>
T sign(T x)
{
    return ((x>=(T)0) ? (T)1 : (T)(-1));
}
template<class T>
T sign2v(T a, T b)
{
    return (sign<T>(b)*y_abs<T>(a));
}
// Displacement fields for pairwise images with overlap regions
template <class T, class Tidx>
class DF
{
public:
    DF(T *in_offsets, Tidx N)
    {
        length = N; // x,y,z
        offsets = NULL;
        sz_image = NULL; // not using
        try
        {
            offsets = new T [length];
            for(Tidx i=0; i<length; i++)
            {
                offsets[i] = in_offsets[i];
            }
        }
        catch (...)
        {
            printf("Fail to allocate memory.\n");
            return;
        }
    }
    DF()
    {
        length = 3; // x, y, z
        offsets = NULL;
        sz_image = NULL;
        try
        {
            offsets = new T [length];
            sz_image = new Tidx [length];
            
            memset(offsets, 0, sizeof(T)*length);
            memset(sz_image, 1, sizeof(Tidx)*length);
        }
        catch (...)
        {
            printf("Fail to allocate memory.\n");
            return;
        }
    }
    
    void setImageSize(Tidx sx, Tidx sy, Tidx sz)
    {
        sz_image[0] = sx;
        sz_image[1] = sy;
        sz_image[2] = sz;
    }
    Tidx getSize()
    {
        return length;
    }
    void clean()
    {
        y_del<T>(offsets);
        y_del<Tidx>(sz_image);
    }
    ~DF(){}
public:
    T *offsets;
    Tidx length;
    Tidx n;
    Tidx pre;
    T coeff;
    string fn_image; // absolute path + file name
    Tidx *sz_image;
    
    // 3D lut
    T rBX, rBY, rBZ;
    T rEX, rEY, rEZ;
    T aBX, aBY, aBZ;
    T aEX, aEY, aEZ;
};
typedef vector< DF<REAL, V3DLONG> > DFList;
// tiles' offsets look up table
template <class T, class Tidx, class DF>
class Y_TLUT 
{
public:
    Tidx vx, vy, vz, vc;
    T minDimx, minDimy, minDimz;
    
    vector<DF> tcList;
    
public:
    Y_TLUT(){}
    ~Y_TLUT(){}
    
public:
    void clut() // construct look up table
    {
        //
        minDimx=INF;
        minDimy=INF;
        minDimz=INF;
        
        T maxDimx=-INF;
        T maxDimy=-INF;
        T maxDimz=-INF;
        
        for(Tidx i=0; i<tcList.size(); i++)
        {
            T startX = tcList.at(i).offsets[0];
            T endX = tcList.at(i).offsets[0]+(T)(tcList.at(i).sz_image[0]-1);
            
            if(minDimx > startX)
                minDimx = startX;
            if(maxDimx < endX)
                maxDimx = endX;
            
            T startY = tcList.at(i).offsets[1];
            T endY = tcList.at(i).offsets[1]+(T)(tcList.at(i).sz_image[1]-1);
            
            if(minDimy > startY)
                minDimy = startY;
            if(maxDimy < endY)
                maxDimy = endY;
            
            T startZ = tcList.at(i).offsets[2];
            T endZ = tcList.at(i).offsets[2]+(T)(tcList.at(i).sz_image[2]-1);
            
            if(minDimz > startZ)
                minDimz = startZ;
            if(maxDimz < endZ)
                maxDimz = endZ;
            
            (&tcList.at(i))->rBX = startX;
            (&tcList.at(i))->rEX = endX;
            
            (&tcList.at(i))->rBY = startY;
            (&tcList.at(i))->rEY = endY;
            
            (&tcList.at(i))->rBZ = startZ;
            (&tcList.at(i))->rEZ = endZ;
        }
        //
        vx = (Tidx)(maxDimx - minDimx + 1.5);
        vy = (Tidx)(maxDimy - minDimy + 1.5);
        vz = (Tidx)(maxDimz - minDimz + 1.5);

        //
        for(Tidx i=0; i<tcList.size(); i++)
        {
            T tile2vi_xs = tcList.at(i).rBX-minDimx;
            T tile2vi_xe = tcList.at(i).rEX-minDimx;
            T tile2vi_ys = tcList.at(i).rBY-minDimy;
            T tile2vi_ye = tcList.at(i).rEY-minDimy;
            T tile2vi_zs = tcList.at(i).rBZ-minDimz;
            T tile2vi_ze = tcList.at(i).rEZ-minDimz;
            
            tcList.at(i).aBX = (0 > tile2vi_xs) ? 0 : tile2vi_xs;
            tcList.at(i).aEX = (vx < tile2vi_xe) ? vx-1 : tile2vi_xe;
            tcList.at(i).aBY = (0 > tile2vi_ys) ? 0 : tile2vi_ys;
            tcList.at(i).aEY = (vy < tile2vi_ye) ? vy-1 : tile2vi_ye;
            tcList.at(i).aBZ = (0 > tile2vi_zs) ? 0 : tile2vi_zs;
            tcList.at(i).aEZ = (vz < tile2vi_ze) ? vz-1 : tile2vi_ze;
            
            tcList.at(i).aEX++;
            tcList.at(i).aEY++;
            tcList.at(i).aEZ++;
        }
    }
    
    void setDimC(Tidx dimc)
    {
        vc = dimc;
    }
    void y_save(string fn) //save virtual image
    {
        FILE *pFileLUT=0;
        pFileLUT = fopen(fn.c_str(),"wt");
        V3DLONG number_tiles = tcList.size();
        //
        fprintf(pFileLUT, "# thumbnail file \n"); // TC_COMMENT1
        fprintf(pFileLUT, "%s \n\n", NULL);
        fprintf(pFileLUT, "# tiles \n"); // TC_COMMENT2
        fprintf(pFileLUT, "%d \n\n", number_tiles);
        fprintf(pFileLUT, "# dimensions (XYZC) \n"); // TC_COMMENT3
        fprintf(pFileLUT, "%ld %ld %ld %ld \n\n", vx, vy, vz, vc);
        fprintf(pFileLUT, "# origin (XYZ) \n"); // TC_COMMENT4
        fprintf(pFileLUT, "%f %f %f \n\n", minDimx, minDimy, minDimz);
        fprintf(pFileLUT, "# resolution (XYZ) \n"); // TC_COMMENT5
        fprintf(pFileLUT, "%lf %lf %lf \n\n", 1.0f, 1.0f, 1.0f);
        fprintf(pFileLUT, "# image coordinates look up table \n"); // TC_COMMENT6
        for(Tidx j=0; j<number_tiles; j++)
        {
            string fns;
            string fn_img_tile = tcList.at(j).fn_image;
            Tidx len_abpath = QFileInfo(QString(fn_img_tile.c_str())).path().length();
            if(len_abpath>1)
                fns = QString(fn_img_tile.c_str()).remove(0, len_abpath+1).toStdString();
            else
                fns = fn_img_tile;
            fprintf(pFileLUT, "%s  ( %f, %f, %lf ) ( %f, %f, %f ) \n", fns.c_str(), tcList.at(j).rBX, tcList.at(j).rBY, tcList.at(j).rBZ, tcList.at(j).rEX, tcList.at(j).rEY, tcList.at(j).rEZ);
        }
        //fprintf(pFileLUT, "\n# MST LUT \n"); // TC_COMMENT7
        fclose(pFileLUT);
    }
    
};
// Define a lookup table
template <class T>
class LUT
{
public:
    LUT(){}
    LUT(T *a, T *b, bool offset_region)
    {
        T len = 3; //start.size();
        if(offset_region)
        {
            init_by_offset(a,b,len);
        }
        else
        {
            init_by_region(a,b,len);
        }
    }
    ~LUT(){}
public:
    void init_by_offset(T *offsets, T *dims, T len)
    {
        try
        {
            start_pos = new T [len];
        }
        catch (...)
        {
            printf("Fail to allocate memory.\n");
            return;
        }
        for(T i=0; i<len; i++)
            start_pos[i] = offsets[i];
        try
        {
            end_pos = new T [len];
        }
        catch (...)
        {
            printf("Fail to allocate memory.\n");
            return;
        }
        for(T i=0; i<len; i++)
            end_pos[i] = start_pos[i] + dims[i] - 1;
    }
    void init_by_region(T *start, T *end, T len)
    {
        try
        {
            start_pos = new T [len];
        }
        catch (...)
        {
            printf("Fail to allocate memory.\n");
            return;
        }
        for(T i=0; i<len; i++)
            start_pos[i] = start[i];
        try
        {
            end_pos = new T [len];
        }
        catch (...)
        {
            printf("Fail to allocate memory.\n");
            return;
        }
        for(T i=0; i<len; i++)
            end_pos[i] = end[i];
    }
    void clear()
    {
        if(start_pos) {delete []start_pos; start_pos=0;}
        if(end_pos) {delete []end_pos; end_pos=0;}
    }
public:
    T *start_pos;
    T *end_pos;
    string fn_img;
};
// define a point structure
template <class T1, class T2>
class _POINT
{
public:
    _POINT(){}
    ~_POINT(){}
public:
    T1 x,y,z;
    T2 intensity;
    string fn;
};
// define a point type
typedef _POINT<V3DLONG, float> POINT;
// Define a indexed data structure
template <class T1, class T2>
class indexed_t
{
public:
    indexed_t(T1 *in_offsets)
    {
        T1 len = 3; //in_offsets.size();
        offsets_sa = NULL;
        hasedge = false;
        
        try
        {
            offsets = new T1 [len];
        }
        catch (...)
        {
            printf("Fail to allocate memory.\n");
            return;
        }
        try
        {
            sz_image = new T1 [len+1]; // X Y Z C
        }
        catch (...)
        {
            printf("Fail to allocate memory.\n");
            return;
        }
        // init
        for(T1 i=0; i<len; i++)
        {
            offsets[i] = in_offsets[i];
            sz_image[i] = 1;
        }
        sz_image[3] = 1;
    }
    
    indexed_t()
    {
        T1 len = 3; // x, y, z
        offsets = NULL;
        hasedge = false;
        
        try
        {
            offsets_sa = new T2 [len];
        }
        catch (...)
        {
            printf("Fail to allocate memory.\n");
            return;
        }
        try
        {
            sz_image = new T1 [len+1]; // X Y Z C
        }
        catch (...)
        {
            printf("Fail to allocate memory.\n");
            return;
        }
        // init
        for(T1 i=0; i<len; i++)
        {
            offsets_sa[i] = 0.0;
            sz_image[i] = 1;
        }
        sz_image[3] = 1;
        
    }
    
    void clean()
    {
        y_del<T1>(offsets);
        y_del<T2>(offsets_sa);
        y_del<T1>(sz_image);
    }
    
    ~indexed_t(){}
public:
    T1 *offsets; // ref
    T1 ref_n; // reference image number
    T1 n;
    T2 score;
    T2 *offsets_sa; // subpixel accuracy
    string fn_image; // absolute path + file name
    T1 *sz_image;
    //	T cn; // child image number
    //	bool cp; // if cp=0 then offsets are curr tile to its child; if cp=1 then offsets are curr tile to its parent
    //	T *offsets_child;
    T1 predecessor; // adjacent prior image number | root's predecessor is -1
    std::vector<T1> preList;
    std::vector<POINT> offsetsList;
    std::vector<REAL> scoreList;
    bool visited; // init by false
    bool hasedge;
    std::vector<indexed_t> record;
    //std::vector<indexed_t> path;
    //indexed_t *parent; // adjacent prior level 1: front -> back (z); level 2: upper -> down (y); level 3: left -> right (x)
};
// swap
template <class Tdata>
void swap2v(Tdata a, Tdata b)
{
    Tdata tmp = a;
    a=b;
    b=tmp;
}
//sort by ascending order
template <class Tdata, class Tidx>
void quickSortAscending(Tdata *a, Tidx l, Tidx r)
{
    if(l>=r) return;
    Tidx i=l;
    Tidx j=r+1;
    Tdata pivot = a[l];
    while(true)
    {
        do{ i = i+1; } while(a[i]<pivot);
        do{ j = j-1; } while(a[j]>pivot);
        if(i>=j) break;
        //swap
        swap2v<Tdata>(a[i], a[j]);
    }
    //
    a[l] = a[j];
    a[j] = pivot;
    //
    quickSortAscending(a, l, j-1);
    quickSortAscending(a, j+1, r);
}
//sort by descending order
template <class Tdata, class Tidx>
void quickSortDescending(Tdata *a, Tidx l, Tidx r)
{
    if(l>=r) return;
    Tidx i=l;
    Tidx j=r;
    Tdata pivot = a[r];
    while(true)
    {
        while(a[i]>=pivot) i++;
        while(a[j]<=pivot) j--;
        if(i>=j) break;
        //swap
        swap2v<Tdata>(a[i], a[j]);
    }
    //
    a[r] = a[i];
    a[i] = pivot;
    qDebug()<<"i j"<<i<<j;
    //
    quickSortDescending(a, i+1, r);
    quickSortDescending(a, l, i-1);
}
// Matrix class
template<class Tdata, class Tidx>
class Y_MAT
{
public:
    Y_MAT(){p=NULL; v=NULL;}
    Y_MAT(Tidx dimx, Tidx dimy, Tdata value)
    {
        // value == 0 is a zero matrix
        // value == 1 is a unitary matrix
        // otherwise create a diagonal matrix
        row = dimx;
        column = dimy;
        p=NULL;
        try
        {
            Tidx N = row*column;
            p=new Tdata [N];
            v=new Tdata *[column];
        }
        catch(...)
        {
            cout<<"Fail to allocate memory for new Matrix!"<<endl;
            return;
        }
        // construct 2d pointer proxy
        for(Tidx j=0; j<column; j++)
        {
            v[j] = p + j*row;
        }
        if(column<row)
        {
            for(Tidx j=0; j<column; j++)
            {
                for(Tidx i=0; i<row; i++)
                {
                    v[j][i] = 0;
                }
                v[j][j] = (Tdata)value;
            }
        }
        else
        {
            for(Tidx i=0; i<row; i++)
            {
                for(Tidx j=0; j<column; j++)
                {
                    v[j][i] = 0;
                }
                v[i][i] = (Tdata)value;
            }
        }
    }
    Y_MAT(Tdata *pInput, Tidx dimx, Tidx dimy)
    {
        p = pInput;
        row = dimx;
        column = dimy;
        // construct 2d pointer proxy
        try
        {
            v=new Tdata *[column];
            for(Tidx j=0; j<column; j++)
            {
                v[j] = p + j*row;
            }
        }
        catch(...)
        {
            cout<<"Fail to allocate memory for new Matrix!"<<endl;
            return;
        }
    }
    ~Y_MAT(){}
public:
    void clone(Y_MAT pIn, bool t)
    {
        if(t) // transpose
        {
            row = pIn.column;
            column = pIn.row;
        }
        else
        {
            row = pIn.row;
            column = pIn.column;
        }
        Tidx N = row*column;
        try
        {
            p=new Tdata [N];
            v=new Tdata *[column];
            for(Tidx j=0; j<column; j++)
            {
                v[j] = p + j*row;
            }
            if(t)
            {
                for(Tidx j=0; j<column; j++)
                {
                    for(Tidx i=0; i<row; i++)
                    {
                        v[j][i] = pIn.v[i][j];
                    }
                }
            }
            else
            {
                for(Tidx i=0; i<N; i++)
                {
                    p[i] = pIn.p[i];
                }
            }
        }
        catch(...)
        {
            cout<<"Fail to allocate memory for new Matrix!"<<endl;
            return;
        }
        return;
    }
    void copy(Y_MAT M)
    {
        clean();
        clone(M, false);
    }
    void clean()
    {
        y_del<Tdata>(p);
        if(v){ delete [] v; v=NULL;} // because v is a 2D proxy pointer of p and p has been deleted
    }
    void prod(Y_MAT M)
    {
        if(column!=M.row)
        {
            cout<<"Matrix cannot multiply because dimensions are not match!"<<endl;
            return;
        }
        Y_MAT T(row,M.column, 0);
        // multiply
        for(Tidx k=0; k<M.column; k++)
        {
            for(Tidx i=0; i<row; i++)
            {
                Tdata sum=0;
                for(Tidx j=0; j<column; j++)
                {
                    sum += v[j][i]*M.v[k][j];
                }
                T.v[k][i] = sum;
            }
        }
        //
        copy(T);
        T.clean();
    }
    void transpose()
    {
        Y_MAT T; // tmp Matrix
        T.clone(*this, true);
        copy(T);
        T.clean();
    }
    void pseudoinverse()
    {
        // A = U*S*V'
        // inv(A) = V*inv(S)*U'
        if(row!=column)
        {
            cout<<"Need a square matrix to decomposition!"<<endl;
            return;
        }
        //
        Tidx N = row;
        // init
        Y_MAT V(N, N, 0);
        Y_MAT S(N, N, 0);
        Y_MAT U;
        U.copy(*this);
        svd(U, S, V);
        // U'
        U.transpose();
        // inv(S)
        for(Tidx i=0; i<N; i++)
        {
            if(S.v[i][i]>EPS)
                S.v[i][i] = 1.0/S.v[i][i];
            else
                S.v[i][i] = 0.0;
        }
        // V*inv(S)*U'
        V.prod(S);
        V.prod(U);
        copy(V);
        //
        V.clean();
        S.clean();
        U.clean();
    }
    // computes (sqrt(a^2 + b^2)) without destructive underflow or overflow
    // "Numerical Recipes" 1992
    Tdata pythagorean(Tdata a, Tdata b)
    {
        Tdata at = fabs(a), bt = fabs(b), ct, result;
        if (at > bt)       { ct = bt / at; result = at * sqrt(1.0 + ct * ct); }
        else if (bt > 0.0) { ct = at / bt; result = bt * sqrt(1.0 + ct * ct); }
        else result = 0.0;
        return result;
    }
    // if A=U*S*V'
    //  INPUT: A (U=A, S=0, V=0)
    // OUTPUT: U, S, V
    // row=column case of the routine SVDCMP from "Numerical Recipes" 1992
    void svd(Y_MAT U, Y_MAT S, Y_MAT V)
    {
        if(row!=column)
        {
            cout<<"Need a square matrix to decomposition!"<<endl;
            return;
        }
        Tidx N = row;
        Tidx flag, i, its, j, jj, k, l, nm;
        Tdata anorm = 0.0, g = 0.0, scale = 0.0;
        Tdata c, f, h, s, x, y, z;
        Tdata *rv1=NULL;
        try
        {
            rv1 = new Tdata [N];
        }
        catch(...)
        {
            cout<<"Fail to allocate memory for temporary variable in svd function!"<<endl;
            y_del<Tdata>(rv1);
            return;
        }
        // Householder reduction to bidiagonal form
        for (i = 0; i < N; i++)
        {
            l = i + 1;
            rv1[i] = scale * g;
            // left-hand reduction
            g = s = scale = 0.0;
            if (i < N)
            {
                for (k = i; k < N; k++)
                    scale += fabs(U.v[i][k]);
                if (scale)
                {
                    for (k = i; k < N; k++)
                    {
                        U.v[i][k] /= scale;
                        s += (U.v[i][k] * U.v[i][k]);
                    }
                    f = U.v[i][i];
                    g = -sign2v<Tdata>(sqrt(s), f);
                    h = f * g - s;
                    U.v[i][i] = f - g;
                    if (i != N - 1)
                    {
                        for (j = l; j < N; j++)
                        {
                            for (s = 0.0, k = i; k < N; k++)
                                s += (U.v[i][k] * U.v[j][k]);
                            f = s / h;
                            for (k = i; k < N; k++)
                                U.v[j][k] += f * U.v[i][k];
                        }
                    }
                    for (k = i; k < N; k++)
                        U.v[i][k] *= scale;
                }
            }
            S.v[i][i] = scale * g;
            // right-hand reduction
            g = s = scale = 0.0;
            if (i < N && i != N - 1)
            {
                for (k = l; k < N; k++)
                    scale += fabs(U.v[k][i]);
                if (scale)
                {
                    for (k = l; k < N; k++)
                    {
                        U.v[k][i] /= scale;
                        s += U.v[k][i] * U.v[k][i];
                    }
                    f = U.v[l][i];
                    g = -sign2v<Tdata>(sqrt(s), f);
                    h = f * g - s;
                    U.v[l][i] = f - g;
                    for (k = l; k < N; k++)
                        rv1[k] = U.v[k][i] / h;
                    if (i != N - 1)
                    {
                        for (j = l; j < N; j++)
                        {
                            for (s = 0.0, k = l; k < N; k++)
                                s += U.v[k][j] * U.v[k][i];
                            for (k = l; k < N; k++)
                                U.v[k][j] += s * rv1[k];
                        }
                    }
                    for (k = l; k < N; k++)
                        U.v[k][i] *= scale;
                }
            }
            anorm = y_max(anorm, (fabs(S.v[i][i]) + fabs(rv1[i])));
        }
        // accumulate the right-hand transformation
        for (i = N - 1; i >= 0; i--)
        {
            if (i < N - 1)
            {
                if (g)
                {
                    for (j = l; j < N; j++)
                        V.v[i][j] = (U.v[j][i] / U.v[l][i]) / g;
                    // double division to avoid underflow
                    for (j = l; j < N; j++)
                    {
                        for (s = 0.0, k = l; k < N; k++)
                            s += (U.v[k][i] * V.v[j][k]);
                        for (k = l; k < N; k++)
                            V.v[j][k] += (s * V.v[i][k]);
                    }
                }
                for (j = l; j < N; j++)
                    V.v[i][j] = V.v[j][i] = 0.0;
            }
            V.v[i][i] = 1.0;
            g = rv1[i];
            l = i;
        }
        // accumulate the left-hand transformation
        for (i = N - 1; i >= 0; i--)
        {
            l = i + 1;
            g = S.v[i][i];
            if (i < N - 1)
                for (j = l; j < N; j++)
                    U.v[j][i] = 0.0;
            if (g)
            {
                g = 1.0 / g;
                if (i != N - 1)
                {
                    for (j = l; j < N; j++)
                    {
                        for (s = 0.0, k = l; k < N; k++)
                            s += (U.v[i][k] * U.v[j][k]);
                        f = (s / U.v[i][i]) * g;
                        for (k = i; k < N; k++)
                            U.v[j][k] += f * U.v[i][k];
                    }
                }
                for (j = i; j < N; j++)
                    U.v[i][j] *= g;
            }
            else
            {
                for (j = i; j < N; j++)
                    U.v[i][j] = 0.0;
            }
            ++U.v[i][i];
        }
        // diagonalize the bidiagonal form
        for (k = N - 1; k >= 0; k--)
        {   // loop over singular values
            for (its = 0; its < 30; its++)
            {   // loop over allowed iterations
                flag = 1;
                for (l = k; l >= 0; l--)
                {   // Test for splitting
                    // Note that rv1[1] is always zero
                    nm = l - 1;
                    if (fabs(rv1[l]) + anorm == anorm)
                    {
                        flag = 0;
                        break;
                    }
                    if (fabs(S.v[nm][nm]) + anorm == anorm)
                        break;
                }
                if (flag)
                {
                    c = 0.0;
                    s = 1.0;
                    for (i = l; i <= k; i++)
                    {
                        f = s * rv1[i];
                        if (fabs(f) + anorm != anorm)
                        {
                            g = S.v[i][i];
                            h = pythagorean(f, g);
                            S.v[i][i] = h;
                            h = 1.0 / h;
                            c = g * h;
                            s = (- f * h);
                            for (j = 0; j < N; j++)
                            {
                                y = U.v[nm][j];
                                z = U.v[i][j];
                                U.v[nm][j] = y*c + z*s;
                                U.v[i][j] = z*c - y*s;
                            }
                        }
                    }
                }
                z = S.v[k][k];
                if (l == k)
                {   //convergence
                    if (z < 0.0)
                    {   // singular value is made nonnegative
                        S.v[k][k] = -z;
                        for (j = 0; j < N; j++)
                            V.v[k][j] = -V.v[k][j];
                    }
                    break;
                }
                if (its >= 30) {
                    y_del<Tdata>(rv1);
                    cout<<"No convergence in 30 svdcmp iterations!"<<endl;
                    return;
                }
                // shift from bottom 2-by-2 minor
                x = S.v[l][l];
                nm = k - 1;
                y = S.v[nm][nm];
                g = rv1[nm];
                h = rv1[k];
                f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
                g = pythagorean(f, 1.0);
                f = ((x - z) * (x + z) + h * ((y / (f + sign2v<Tdata>(g, f))) - h)) / x;
                // next QR transformation
                c = s = 1.0;
                for (j = l; j <= nm; j++)
                {
                    i = j + 1;
                    g = rv1[i];
                    y = S.v[i][i];
                    h = s * g;
                    g = c * g;
                    z = pythagorean(f, h);
                    rv1[j] = z;
                    c = f / z;
                    s = h / z;
                    f = x * c + g * s;
                    g = g * c - x * s;
                    h = y * s;
                    y = y * c;
                    for (jj = 0; jj < N; jj++)
                    {
                        x = V.v[j][jj];
                        z = V.v[i][jj];
                        V.v[j][jj] = x*c + z*s;
                        V.v[i][jj] = z*c - x*s;
                    }
                    z = pythagorean(f, h);
                    S.v[j][j] = z;
                    if (z)
                    {
                        z = 1.0 / z;
                        c = f * z;
                        s = h * z;
                    }
                    f = (c * g) + (s * y);
                    x = (c * y) - (s * g);
                    for (jj = 0; jj < N; jj++)
                    {
                        y = U.v[j][jj];
                        z = U.v[i][jj];
                        U.v[j][jj] = y*c + z*s;
                        U.v[i][jj] = z*c - y*s;
                    }
                }
                rv1[l] = 0.0;
                rv1[k] = f;
                S.v[k][k] = x;
            }
        }
        y_del<Tdata>(rv1);
        return;
    }
    
public:
    Tdata *p;
    Tdata **v;
    Tidx row, column;
};
// Virtual Image Class
template <class T1, class T2, class indexed_t, class LUT>
class Y_VIM 
{
public:
    //init
    // creating a hash table
    Y_VIM(list<string> imgList, T2 dims)
    {
        // finding best global alignment
    }
    Y_VIM()
    {
        pVim=NULL;
        sz=NULL;
        lut=NULL;
        min_vim=NULL; max_vim=NULL;
        try
        {
            sz = new T2 [4];
        }
        catch (...)
        {
            printf("Fail to allocate memory for sz.\n");
            return;
        }
        b_thumbnailcreated = false;
    }
    // destructor
    ~Y_VIM(){}
public:
    // error func
    bool error(ifstream *pFileLUT)
    {
        //QMessageBox::information(0, "TC file reading", QObject::tr("Your .tc file is illegal."));
        
        cout<<"Your stitching configuration (.tc) file is illegal."<<endl;
        pFileLUT->close();
        return false;
    }
    
    //load a virtual image
    bool y_load(string fn)
    {
        QString curPath = QFileInfo(fn.c_str()).path();
        
        ifstream pFileLUT(fn.c_str());
        string str;
        char letter;
        T2 start[3], end[3];
        char buf[2048];
        string fn_str;
        if(pFileLUT.is_open())
        {
            //
            pFileLUT >> letter;
            if(letter=='#')
                getline(pFileLUT, str); // read comments line
            if(strcmp(str.c_str(), TC_COMMENT1))
            {
                return error(&pFileLUT);
            }
            else
            {
                // thumbnail file name
                pFileLUT >> fn_thumbnail;
            }
            do
            {
                pFileLUT >> letter;
            }
            while(letter!='#' && !pFileLUT.eof());
            
            if(pFileLUT.eof()) return error(&pFileLUT);
            getline(pFileLUT, str); // read comments line
            if(strcmp(str.c_str(), TC_COMMENT2))
            {
                return error(&pFileLUT);
            }
            else
            {
                // tiles
                pFileLUT >> number_tiles;
            }
            do
            {
                pFileLUT >> letter;
            }
            while(letter!='#' && !pFileLUT.eof());
            
            if(pFileLUT.eof()) return error(&pFileLUT);
            getline(pFileLUT, str); // read comments line
            if(strcmp(str.c_str(), TC_COMMENT3))
            {
                return error(&pFileLUT);
            }
            else
            {
                // dimensions
                pFileLUT >> sz[0] >> sz[1] >> sz[2] >> sz[3];
            }
            do
            {
                pFileLUT >> letter;
            }
            while(letter!='#' && !pFileLUT.eof());
            
            if(pFileLUT.eof()) return error(&pFileLUT);
            getline(pFileLUT, str); // read comments line
            if(strcmp(str.c_str(), TC_COMMENT4))
            {
                return error(&pFileLUT);
            }
            else
            {
                // origins
                pFileLUT >> origin_x >> origin_y >> origin_z;
            }
            do
            {
                pFileLUT >> letter;
            }
            while(letter!='#' && !pFileLUT.eof());
            
            if(pFileLUT.eof()) return error(&pFileLUT);
            getline(pFileLUT, str); // read comments line
            if(strcmp(str.c_str(), TC_COMMENT5))
            {
                return error(&pFileLUT);
            }
            else
            {
                // resolutions
                pFileLUT >> rez_x >> rez_y >> rez_z;
            }
            do
            {
                pFileLUT >> letter;
            }
            while(letter!='#' && !pFileLUT.eof());
            
            if(pFileLUT.eof()) return error(&pFileLUT);
            getline(pFileLUT, str); // read comments line
            if(strcmp(str.c_str(), TC_COMMENT6))
            {
                return error(&pFileLUT);
            }
            else
            {
                // lut
                lut = new LUT [number_tiles];
                T2 count=0;
                while( !pFileLUT.eof() && getline(pFileLUT, str) && count<number_tiles)
                {
                    istringstream iss(str);
                    
                    if(iss.fail() || iss.eof())
                        continue;
                    
                    iss >> fn_str;
                    
                    //
                    iss >> buf; iss >> start[0];
                    iss >> buf; iss >> start[1];
                    iss >> buf; iss >> start[2];
                    
                    iss >> buf;
                    
                    iss >> buf; iss >> end[0];
                    iss >> buf; iss >> end[1];
                    iss >> buf; iss >> end[2];
                    
                    lut[count] = LUT(start, end, false);
                    lut[count].fn_img = fn_str; // relative path
                    
                    count++;
                }
            }
            if(!pFileLUT.eof()) // compatible with old .tc file
            {
                do
                {
                    pFileLUT >> letter;
                }
                while(letter!='#' && !pFileLUT.eof());
                
                if(!pFileLUT.eof())
                {
                    getline(pFileLUT, str); // read comments line
                    
                    if(strcmp(str.c_str(), TC_COMMENT7) == 0)
                    {
                        for(V3DLONG i=0; i<number_tiles; i++)
                        {
                            indexed_t t;  //
                            
                            t.n = i;
                            t.predecessor = -1;
                            
                            QString fn = curPath;
                            t.fn_image = fn.append("/").append( QString(lut[i].fn_img.c_str()) ).toStdString(); // absolute path
                            
                            tilesList.push_back(t);
                        }
                        
                        T2 count=0;
                        
                        while( !pFileLUT.eof() && getline(pFileLUT, str) && count<number_tiles)
                        {
                            istringstream iss(str);
                            
                            if(iss.fail() || iss.eof())
                                continue;
                            
                            T2 num;
                            iss >> num;
                            iss >> tilesList.at(num).predecessor;
                            
                            iss >> tilesList.at(num).offsets_sa[0];
                            iss >> tilesList.at(num).offsets_sa[1];
                            iss >> tilesList.at(num).offsets_sa[2];
                            
                            count++;
                        }
                    }
                }
                else
                {
                    cout<<"Warning: This stitching configuration file is old version without MST info."<<endl;
                }
            }
        }
        else
        {
            cout << "Unable to open the file";
            pFileLUT.close();
            return false;
        }
        pFileLUT.close();
        // adjusting
        T2 len = 3; // x y z
        try
        {
            min_vim = new T2 [len];
            max_vim = new T2 [len];
        }
        catch (...)
        {
            printf("Fail to allocate memory.\n");
            return false;
        }
        for(T2 i=0; i<len; i++)
        {
            min_vim[i] = 0; max_vim[i] = 0;
        }
        for(T2 i=0; i<number_tiles; i++)
        {
            for(T2 j=0; j<len; j++)
            {
                if(lut[i].start_pos[j] < min_vim[j])
                    min_vim[j] = lut[i].start_pos[j];
                if(lut[i].end_pos[j] > max_vim[j])
                    max_vim[j] = lut[i].end_pos[j];
            }
        }
        return true;
    }
    //save as a virtual image
    void y_save(string fn)
    {
        FILE *pFileLUT=0;
        pFileLUT = fopen(fn.c_str(),"wt");
        // thumbnail image not existing
        if(!b_thumbnailcreated)
            strcpy(fn_thumbnail, y_createthumbnail()); // create a thumbnail image
        fprintf(pFileLUT, "# thumbnail file \n"); // TC_COMMENT1
        fprintf(pFileLUT, "%s \n\n", fn_thumbnail);
        fprintf(pFileLUT, "# tiles \n"); // TC_COMMENT2
        fprintf(pFileLUT, "%d \n\n", number_tiles);
        if(!max_vim || !min_vim)
        {
            QMessageBox::information(0, "TC file writing", QObject::tr("Your .tc data structure is illegal."));
            return;
        }
        fprintf(pFileLUT, "# dimensions (XYZC) \n"); // TC_COMMENT3
        fprintf(pFileLUT, "%ld %ld %ld %ld \n\n", max_vim[0]-min_vim[0]+1, max_vim[1]-min_vim[1]+1, max_vim[2]-min_vim[2]+1, sz[3]);
        // init
        origin_x = min_vim[0]; origin_y = min_vim[1]; origin_z = min_vim[2];
        rez_x = 1; rez_y= 1; rez_z = 1;
        fprintf(pFileLUT, "# origin (XYZ) \n"); // TC_COMMENT4
        fprintf(pFileLUT, "%lf %lf %lf \n\n", origin_x, origin_y, origin_z);
        fprintf(pFileLUT, "# resolution (XYZ) \n"); // TC_COMMENT5
        fprintf(pFileLUT, "%lf %lf %lf \n\n", rez_x, rez_y, rez_z);
        fprintf(pFileLUT, "# image coordinates look up table \n"); // TC_COMMENT6
        for(T2 j=0; j<number_tiles; j++)
        {
            string fns;
            string fn_img_tile = lut[j].fn_img;
            T2 len_abpath = QFileInfo(QString(fn_img_tile.c_str())).path().length();
            if(len_abpath>1)
                fns = QString(fn_img_tile.c_str()).remove(0, len_abpath+1).toStdString();
            else
                fns = fn_img_tile;
            fprintf(pFileLUT, "%s  ( %ld, %ld, %ld ) ( %ld, %ld, %ld ) \n", fns.c_str(), lut[j].start_pos[0], lut[j].start_pos[1], lut[j].start_pos[2], lut[j].end_pos[0], lut[j].end_pos[1], lut[j].end_pos[2]);
        }
        
        fprintf(pFileLUT, "\n# MST LUT \n"); // TC_COMMENT7
        for(T2 j=1; j<number_tiles; j++)
        {
            fprintf(pFileLUT, "%ld %ld %ld %ld %ld \n", tilesList.at(j).n, tilesList.at(j).predecessor, tilesList.at(j).record.at(0).offsets[0], tilesList.at(j).record.at(0).offsets[1], tilesList.at(j).record.at(0).offsets[2]);
        }
        fclose(pFileLUT);
    }
    // when add a new one into tileList, need to update the whole tileList
    void y_update()
    {
        // compute accumulate offsets from path list
        // t.ref_n = ref_n
        // t.parent = parent
        // t.record.at(0).offsets = offsets to ref

        T2 i,NTILES = tilesList.size();
        ref_n = 0;

        for(i=0; i<NTILES; i++)
        {
            tilesList.at(i).visited = false;
        }

        for(i=0; i<NTILES; i++)
        {
            T2 offsets[3];

            if(i==ref_n)
            {
                offsets[0] = 0;
                offsets[1] = 0;
                offsets[2] = 0;
                indexed_t t(offsets);
                (&tilesList.at(i))->record.push_back(t);
            }
            else
            {
                // ref
                (&tilesList.at(i))->ref_n = ref_n;

                int current = tilesList.at(i).n;
                int previous = tilesList.at(i).predecessor;

                //
                offsets[0] = tilesList.at(i).offsets[0];
                offsets[1] = tilesList.at(i).offsets[1];
                offsets[2] = tilesList.at(i).offsets[2];
                while(previous!=-1)
                {
                    if(tilesList.at(current).visited)
                    {
                        break;
                    }

                    offsets[0] += tilesList[previous].record[0].offsets[0];
                    offsets[1] += tilesList[previous].record[0].offsets[1];
                    offsets[2] += tilesList[previous].record[0].offsets[2];

                    //
                    current = previous;
                    previous = tilesList.at(current).predecessor;
                }

                indexed_t t(offsets);
                (&tilesList.at(i))->record.push_back(t);

                (&tilesList.at(i))->visited = true;
            }
        }
    }
    // make a visual image real and be loaded into memory
    void y_visualize(T2 *start, T2 *end)
    {
    }
    // make a visual image real and be loaded into memory
    void y_visualize()
    {
    }
    // show a header info
    void y_info()
    {
    }
    // point navigation
    POINT y_navigate(POINT p)
    {
        return p;
    }
    // create thumbnail file
    const char *y_createthumbnail()
    {
        static char fn[2048] = "NULL";
        // to do
        return fn;
    }
    // construct lookup table given adjusted tilesList
    void y_clut(T2 n)
    {
        number_tiles = n; // init
        sz[3] = tilesList.at(0).sz_image[3];
        lut = new LUT [n];
        for(T2 i=0; i<n; i++)
        {
            lut[i] = LUT(tilesList.at(i).offsets, tilesList.at(i).sz_image, true);
            lut[i].fn_img = tilesList.at(i).fn_image;
        }
        // suppose image dimension is unsigned
        T2 len = 3;
        try
        {
            min_vim = new T2 [len];
            max_vim = new T2 [len];
        }
        catch (...)
        {
            printf("Fail to allocate memory.\n");
            return;
        }
        for(T2 i=0; i<len; i++)
        {
            min_vim[i] = 0; max_vim[i] = 0;
        }
        for(T2 i=0; i<n; i++)
        {
            for(T2 j=0; j<len; j++)
            {
                if(lut[i].start_pos[j] < min_vim[j])
                    min_vim[j] = lut[i].start_pos[j];
                if(lut[i].end_pos[j] > max_vim[j])
                    max_vim[j] = lut[i].end_pos[j];
            }
        }
    }
    void y_constructLUT()
    {
        T2 i,j;
        number_tiles = tilesList.size();

        // rename the n and predecessor
        for(i=0; i<number_tiles; i++)
        {
            (&tilesList.at(i))->visited = false;
        }
        for(j=0; j<number_tiles; j++)
        {
            T2 cur = tilesList[j].n;
            for(i=0; i<number_tiles; i++)
            {
                T2 pre = tilesList[i].predecessor;

                if(pre==cur && !tilesList[i].visited)
                {
                    (&tilesList.at(i))->predecessor = j;
                    (&tilesList.at(i))->visited = true;
                }
            }
            (&tilesList.at(j))->n = j;
        }

        // update offsets
        y_update();

        // construct lut
        y_clut(number_tiles);

    }

    void y_delete(T2 n)
    {
        T2 size = tilesList.size();

        if(n<0 || n>=size)
        {
            cout<<"invalid input number"<<endl;
            return;
        }

        T2 i, j, sizerecord;
        if(size>2)
        {
            if(n<size-1)
            {
                for(i=n+1; i<size; i++)
                {
                    tilesList.at(i).n--;
                    if(tilesList.at(i).predecessor>n)
                        tilesList.at(i).predecessor--;

                    if(tilesList.at(i).ref_n>n)
                        tilesList.at(i).ref_n--;

                    sizerecord = tilesList.at(i).record.size();
                    for(j=0; j<sizerecord; j++)
                    {
                        if(tilesList.at(i).record.at(j).n>n)
                            tilesList.at(i).record.at(j).n--;
                    }

                }
            }

            tilesList.erase(tilesList.begin()+n,tilesList.begin()+n+1);
        }

        //
        return;
    }

    void y_reverse()
    {
        T2 n = tilesList.size();

        if(n>1)
        {
            tilesList.reverse(tilesList.begin(), tilesList.end());
        }

        return;
    }

    T2 y_find(string fn)
    {
        T2 i,n = tilesList.size();
        T2 len = fn.length();

        if(n>1)
        {
            for(i=0; i<n; i++)
            {
                unsigned foundstr = fn.find(tilesList.at(i).fn_image);
                if(foundstr<len)
                {
                    return i;
                }
            }
        }

        return -1;
    }

    T2 y_ref()
    {
        T2 i,n=tilesList.size();
        T2 minsz = INF;

        ref_n = -1;
        if(n>1)
        {
            for(i=0; i<n; i++)
            {
                if(tilesList.at(i).record.size()<minsz)
                {
                    ref_n = i;
                    minsz = tilesList.at(i).record.size();
                }
            }
        }

        return ref_n;
    }

    bool y_ks(vector<indexed_t> tilesList)
    {
        T2 i,size = tilesList.size();
        for(i=0; i<size; i++)
        {
            if(!tilesList.at(i).visited)
            {
                return true;
            }
        }
        return false;
    }

    bool y_ks(vector<bool> visited)
    {
        T2 i,size = visited.size();
        for(i=0; i<size; i++)
        {
            if(!visited[i])
            {
                return true;
            }
        }
        return false;
    }

    int y_mstKruskal(T2 num_edges)
    {
        //
        if(num_edges<=0)
        {
            cout<<"No edges"<<endl;
            return -1;
        }

        //
        qDebug()<<"test ...";
        vector<indexed_t> mst;
        mst.clear();
        qDebug()<<"test ... ...";

        //
        T2 i,j,size = tilesList.size();

        //
        if(size<1)	return -1;
        else if(size==1) return 0;
        else if(size==2)
        {
            //step 1. choose ref as anchor image
            T2 ref = y_ref();
            (&tilesList.at(ref))->predecessor = -1;
            (&tilesList.at(ref))->visited = true;
            //step 2. adjust the other's offsets
            T2 other = 1-ref;
            (&tilesList.at(other))->predecessor = 0;
            (&tilesList.at(other))->visited = true;
            (&tilesList.at(other))->offsets[0] = tilesList.at(other).record.at(0).offsets[0]; // tilesList.at(other).record.at(0).n == ref
            (&tilesList.at(other))->offsets[1] = tilesList.at(other).record.at(0).offsets[1];
            (&tilesList.at(other))->offsets[2] = tilesList.at(other).record.at(0).offsets[2];

            return 0;
        }
        else
        {
            qDebug()<<"test ... b";
            //init
            typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::property < boost::edge_weight_t, REAL > > Graph;
            typedef boost::graph_traits < Graph >::edge_descriptor Edge;
            typedef boost::graph_traits < Graph >::vertex_descriptor Vertex;
            typedef std::pair<int, int> E;
            qDebug()<<"test ... bb";

            E *edge_array = NULL;
            REAL *weights = NULL;

            qDebug()<<"test ... n";
            y_new<E, T2>(edge_array, num_edges);
            y_new<REAL, T2>(weights, num_edges);
            vector<bool> visited;
            qDebug()<<"test ... nn";

            T2 idx=0;
            for(i=0; i<size; i++)
            {
                visited.push_back(false); // init
                for(j=0; j<tilesList.at(i).record.size(); j++)
                {
                    edge_array[idx] = E(i,tilesList.at(i).record.at(j).n);
                    weights[idx++] = -tilesList.at(i).record.at(j).score; // maximum spanning tree
                }
            }

#if defined(BOOST_MSVC) && BOOST_MSVC <= 1300
            Graph g(size);
            boost::property_map<Graph, boost::edge_weight_t>::type weightmap = get(boost::edge_weight, g);
            for (std::size_t j = 0; j < num_edges; ++j) {
                Edge e; bool inserted;
                boost::tie(e, inserted) = add_edge(edge_array[j].first, edge_array[j].second, g);
                weightmap[e] = weights[j];
            }
#else
            Graph g(edge_array, edge_array + num_edges, weights, size);
#endif
            boost::property_map < Graph, boost::edge_weight_t >::type weight = get(boost::edge_weight, g);
            std::vector < Edge > spanning_tree;
            boost::kruskal_minimum_spanning_tree(g, std::back_inserter(spanning_tree));

            std::vector < Edge >::iterator ei,ej;
            std::vector < Edge >::iterator ebegin = spanning_tree.begin();

            T2 ref = target(*ebegin, g);

            qDebug()<<"test ... 1 ... ";

            if(ref>=0)
            {
                T2 offset[3];
                offset[0] = 0;
                offset[1] = 0;
                offset[2] = 0;

                indexed_t t(offset);
                t.n = tilesList.at(ref).n;
                t.ref_n = -1;
                t.predecessor = -1;
                t.fn_image.assign(tilesList.at(ref).fn_image.c_str());
                t.score = 0;
                t.sz_image[0] = tilesList.at(ref).sz_image[0];
                t.sz_image[1] = tilesList.at(ref).sz_image[1];
                t.sz_image[2] = tilesList.at(ref).sz_image[2];

                mst.push_back(t);
                visited[t.n] = true; //
            }

            qDebug()<<"test ... 11 ... ";

            T2 cursize = 0;
            while(y_ks(visited))
            {
                qDebug()<<"test ... 111 ... "<<mst.size();

                if(mst.size()>cursize)
                    cursize = mst.size();
                else
                    break;

                for(T2 imst=0; imst<mst.size(); imst++)
                {
                    T2 cur = mst[imst].n;

                    for(ei=ebegin; ei!=spanning_tree.end(); ++ei)
                    {
                        T2 offset[3];
                        T2 src=source(*ei, g);
                        T2 tar=target(*ei, g);

                        if(src==cur)
                        {
                            if(!visited[tar])
                            {
                                visited[tar] = true;

                                T2 n2, nrecord = tilesList.at(src).record.size();
                                if(nrecord>0)
                                {
                                    n2 = -1;
                                    for(T2 k=0; k<nrecord; k++)
                                    {
                                        if(tilesList.at(src).record.at(k).n == tar)
                                        {
                                            n2 = k;
                                        }
                                    }

                                    if(n2>=0) // adjacent
                                    {
                                        offset[0] = -tilesList.at(src).record.at(n2).offsets[0];
                                        offset[1] = -tilesList.at(src).record.at(n2).offsets[1];
                                        offset[2] = -tilesList.at(src).record.at(n2).offsets[2];
                                    }
                                    else
                                    {
                                        offset[0] = 0;
                                        offset[1] = 0;
                                        offset[2] = 0;
                                    }
                                }

                                indexed_t t(offset);
                                t.n = tilesList.at(tar).n;
                                t.ref_n = ref;
                                t.predecessor = tilesList.at(src).n;
                                t.fn_image.assign(tilesList.at(tar).fn_image.c_str());
                                t.score = weight[*ei];
                                t.sz_image[0] = tilesList.at(tar).sz_image[0];
                                t.sz_image[1] = tilesList.at(tar).sz_image[1];
                                t.sz_image[2] = tilesList.at(tar).sz_image[2];

                                mst.push_back(t);
                            }
                        }

                        if(tar==cur)
                        {
                            if(!visited[src])
                            {
                                visited[src] = true;

                                T2 n2, nrecord = tilesList.at(src).record.size();
                                if(nrecord>0)
                                {
                                    n2 = -1;
                                    for(T2 k=0; k<nrecord; k++)
                                    {
                                        if(tilesList.at(src).record.at(k).n == tar)
                                        {
                                            n2 = k;
                                        }
                                    }

                                    if(n2>=0) // adjacent
                                    {
                                        offset[0] = tilesList.at(src).record.at(n2).offsets[0];
                                        offset[1] = tilesList.at(src).record.at(n2).offsets[1];
                                        offset[2] = tilesList.at(src).record.at(n2).offsets[2];
                                    }
                                    else
                                    {
                                        offset[0] = 0;
                                        offset[1] = 0;
                                        offset[2] = 0;
                                    }
                                }

                                indexed_t t(offset);
                                t.n = tilesList.at(src).n;
                                t.ref_n = ref;
                                t.predecessor = tilesList.at(tar).n;
                                t.fn_image.assign(tilesList.at(src).fn_image.c_str());
                                t.score = weight[*ei];
                                t.sz_image[0] = tilesList.at(src).sz_image[0];
                                t.sz_image[1] = tilesList.at(src).sz_image[1];
                                t.sz_image[2] = tilesList.at(src).sz_image[2];

                                mst.push_back(t);
                            }
                        }
                    }
                }
            }

            // de-alloc
            y_del<E>(edge_array);
            y_del<REAL>(weights);
            visited.clear();

            //
            tilesList.clear();
            tilesList.assign(mst.begin(),mst.end());

            return 0;
        }
    }

    int y_mstPrim(vector<indexed_t> &tilesList)
    {
        //
        T1 size = tilesList.size();
        if(size<1)	return -1;
        else if(size==1) return 0;
        else if(size==2)
        {
            //step 1. choose 0 as anchor image
            (&tilesList.at(0))->predecessor = -1;
            (&tilesList.at(0))->visited = true;
            //step 2. adjust 1's offsets
            POINT offsets;
            (&tilesList.at(1))->predecessor = 0;
            (&tilesList.at(1))->visited = true;
            (&tilesList.at(1))->hasedge = true;

            offsets.x = tilesList.at(1).record.at(0).offsets[0];
            offsets.y = tilesList.at(1).record.at(0).offsets[1];
            offsets.z = tilesList.at(1).record.at(0).offsets[2];

            (&tilesList.at(1))->scoreList.push_back(tilesList.at(1).record.at(0).score);
            (&tilesList.at(1))->offsetsList.push_back(offsets);
            (&tilesList.at(1))->preList.push_back(0);
        }
        else
        {
            // Prim's algorithm
            //-------------------------------------------------------------
            //1. Start with a tree which contains only one node.
            //2. Identify a node (outside the tree) which is closest to the tree and add the minimum weight edge from that node to some node in the tree and incorporate the additional node as a part of the tree.
            //3. If there are less then n – 1 edges in the tree, go to 2
            //
            //step 1. choose 0 as anchor image
            (&tilesList.at(0))->predecessor = -1;
            (&tilesList.at(0))->visited = true;
            //step 2.
            //func extractHighestScoreEdge()
            //step 3.
            T1 cnt=0;
            T1 nedge=tilesList.size()-1;

            while(cnt<nedge)
            {
                T2 ni, n1, n2;
                T1 max_score=0, score; // [0, 1]
                POINT offsets;
                T2 mse_node; // corresponding maxmum score edge
                T2 parent, previous;

                // step 2.
                for(T2 i=0; i<size; i++)
                {
                    //
                    if(!tilesList.at(i).visited) continue;
                    ni = i;

                    for(T2 j=1; j<size; j++)
                    {
                        if(tilesList.at(j).visited) continue;
                        n1 = ni;
                        n2 = j;
                        // let n1>n2
                        if(n1<n2)
                        {
                            T2 tmp = n1;
                            n1=n2;
                            n2=tmp;
                        }

                        //
                        for(T2 k=0; k<tilesList.at(n1).record.size(); k++)
                        {
                            if(n2==tilesList.at(n1).record.at(k).n && tilesList.at(n1).record.at(k).score > max_score)
                            {
                                max_score = tilesList.at(n1).record.at(k).score;
                                mse_node = j; previous = i; parent = n2;
                                offsets.x = tilesList.at(n1).record.at(k).offsets[0];
                                offsets.y = tilesList.at(n1).record.at(k).offsets[1];
                                offsets.z = tilesList.at(n1).record.at(k).offsets[2];
                                score = max_score;
                            }
                        }

                    } // j
                }// i

                // add new node to mst
                T2 sn1=mse_node, sn2=previous, coef=1;
                if(sn1<sn2)
                {
                    T2 tmp = sn1;
                    sn1=sn2;
                    sn2=tmp;
                    coef = -1;
                }
                //
                qDebug()<<"mst: current "<<sn1<<"previous "<<parent<<"score "<<score;

                (&tilesList.at(sn1))->offsetsList.push_back(offsets);
                (&tilesList.at(sn1))->preList.push_back(parent);
                (&tilesList.at(sn1))->scoreList.push_back(score);

                (&tilesList.at(sn1))->visited = true;
                (&tilesList.at(parent))->visited = true;
                (&tilesList.at(sn1))->hasedge = true;

                cnt++;
            }
        }
        return 0;
    }
    void y_clear()
    {
        if(pVim) {delete []pVim; pVim=0;}
        if(sz) {delete []sz; sz=0;}
        if(min_vim) {delete []min_vim; min_vim=0;}
        if(max_vim) {delete []max_vim; max_vim=0;}
        if(lut) {delete []lut; lut=0;}
    }
public:
    T1 *pVim;
    T2 *sz;
    vector<indexed_t> tilesList;
    bitset<3> relative_dir; // 000 'f', 'u', 'l' ; 111 'b', 'd', 'r'; relative[2] relative[1] relative[0]
    LUT *lut;
    T2 *min_vim, *max_vim;
    T2 number_tiles;
    // record in .tc file
    double origin_x, origin_y, origin_z; // (0, 0, 0)
    double rez_x, rez_y, rez_z; // (1, 1, 1)
    // thumbnail file
    char fn_thumbnail[2048];
    bool b_thumbnailcreated;
    T2 ref_n;
};
// configuration prior
template <class T>
class CONF_INFO
{
public:
    CONF_INFO(){}
    ~CONF_INFO(){}
public:
    T row, col;
    string fn_img;
};
// usually T1 is assigned as unsigned char (uint8) and T2 V3DLONG (unsigned V3DLONG int)
template <class T1, class T2>
class Y_IMAGE 
{
public:
    //initialize a Y_IMAGE from a 1d pointer
    Y_IMAGE(T1 *pIn, T2 *szIn)
    {
        flag_refer=true;
        pImg=pIn; sz=szIn;
    }
    //new a Y_IMAGE
    Y_IMAGE(T2 *szIn, int len=4)
    {
        pImg=0; sz=0;
        flag_refer=false;
        sz = szIn;
        T2 totalsz=0;
        for(T2 i=0; i<len; i++)
        {
            if (i==0) totalsz=sz[0];
            else
                totalsz *= sz[i];
        }
        if (totalsz>0)
            pImg=new T1 [totalsz];
        else
        {
            printf("The sz info is incorrect.\n");
            return;
        }
        
        if (!pImg)
        {
            printf("Fail to allocate memory.\n");
            return;
        }
        else
        {
            for(T2 i=0; i<totalsz; i++)
            {
                pImg[i] = 0;
            }
        }
    }
    ~Y_IMAGE()
    {
        if(!flag_refer && pImg)
        {
            delete []pImg; pImg=0;
        }
    }
public:
    T1 *pImg;
    T2 *sz;
    bool flag_refer;
};
// define Y_IMG types
typedef Y_IMAGE<REAL, V3DLONG> Y_IMG_REAL;
typedef Y_IMAGE<unsigned char, V3DLONG> Y_IMG_UINT8;
typedef Y_IMAGE<unsigned short, V3DLONG> Y_IMG_UINT16;
// define a peak data structure of phase correlation
template <class T>
struct P
{
    T x,y,z;
    REAL value;
};
typedef	P<V3DLONG> PEAKS; 
typedef	P<REAL> rPEAKS; 
// define PEAKSLIST type
typedef	std::vector<PEAKS> PEAKSLIST; 
typedef	std::vector<rPEAKS> rPEAKSLIST; 
//NCC FFT-based using sum-table
//http://www.idiom.com/~zilla/Work/nvisionInterface/nip.html
template <class T1, class T2>
class NST
{
public:
    NST(T2 *sz, T1 *f, T2 even_odd, bool fftwf_in_place, T2 dims)
    {
        if(dims == 2)
            initSumTable2D(sz, f);
        else if (dims == 3)
            initSumTable3D(sz, f, even_odd, fftwf_in_place);
        else if (dims > 3)
            initSumTableND(sz, f);
        else
        {
            printf("Dimensions should be 2, 3, or any positive integer greater than 3.\n");
            return;
        }
    }
    ~NST()
    {
        if(sum1) {delete []sum1; sum1=0;}
        if(sum2) {delete []sum2; sum2=0;}
    }
    // init 2D sum tables
    void initSumTable2D(T2 *sz2d, T1 *f)
    {
        T2 sx=sz2d[0], sy=sz2d[1];
        T2 len=sx*sy;
        try
        {
            sum1 = new T1 [len];
        }
        catch (...)
        {
            printf("Fail to allocate memory.\n");
            return;
        }
        try
        {
            sum2 = new T1 [len];
        }
        catch (...)
        {
            printf("Fail to allocate memory.\n");
            return;
        }

        for(T2 i=0; i<len; i++)
        {
            sum1[i] = 0; sum2[i] = 0;
        }
        for(T2 j=0; j<sy; j++)
        {
            T2 offset_j = j*sx;
            for(T2 i=0; i<sx; i++)
            {
                T2 idx = offset_j + i;
                if(i==0 && j==0)
                {
                    sum1[idx] = f[idx];
                    sum2[idx] = f[idx]*f[idx];
                }
                else if(i==0 && j>0)
                {
                    sum1[idx] = f[idx] + sum1[(j-1)*sx + i];
                    sum2[idx] = f[idx]*f[idx] + sum2[(j-1)*sx + i];
                }
                else if(i>0 && j==0)
                {
                    sum1[idx] = f[idx] + sum1[offset_j + i-1];
                    sum2[idx] = f[idx]*f[idx] + sum2[offset_j + i-1];
                }
                else
                {
                    sum1[idx] = f[idx] + sum1[(j-1)*sx + i] + sum1[offset_j + i-1] -  sum1[(j-1)*sx + i-1];
                    sum2[idx] = f[idx]*f[idx] + sum2[(j-1)*sx + i] + sum2[offset_j + i-1] - sum2[(j-1)*sx + i-1];
                }
            }
        }
    }
    // init 3D sum tables
    void initSumTable3D(T2 *sz3d, T1 *f, T2 even_odd, bool fftwf_in_place)
    {
        T2 sx_f=sz3d[0], sy=sz3d[1], sz=sz3d[2];
        T2 sx;
        if(fftwf_in_place)
            sx = sx_f - (2-even_odd);  //2*(sx_f/2-1); // fftwf_in_place
        else
            sx = sx_f;
        T2 len=sx*sy*sz;
        try
        {
            sum1 = new T1 [len];
        }
        catch (...)
        {
            printf("Fail to allocate memory.\n");
            return;
        }
        try
        {
            sum2 = new T1 [len];
        }
        catch (...)
        {
            printf("Fail to allocate memory.\n");
            return;
        }
        for(T2 i=0; i<len; i++)
        {
            sum1[i] = 0; sum2[i] = 0;
        }
        for(T2 k=0; k<sz; k++)
        {
            T2 offset_k = k*sx*sy;
            T2 offset_f_k = k*sx_f*sy;
            for(T2 j=0; j<sy; j++)
            {
                T2 offset_j = offset_k + j*sx;
                T2 offset_f_j = offset_f_k + j*sx_f;
                for(T2 i=0; i<sx; i++)
                {
                    T2 idx = offset_j + i;
                    T2 idx_f = offset_f_j + i;
                    if(i==0 && j==0 && k==0)
                    {
                        sum1[idx] = f[idx_f];
                        sum2[idx] = f[idx_f]*f[idx_f];
                    }
                    else if(i>0 && j==0 && k==0)
                    {
                        sum1[idx] = f[idx_f] + sum1[offset_j + i-1];
                        sum2[idx] = f[idx_f]*f[idx_f] + sum2[offset_j + i-1];
                    }
                    else if(i==0 && j>0 && k==0)
                    {
                        sum1[idx] = f[idx_f] + sum1[offset_k + (j-1)*sx + i];
                        sum2[idx] = f[idx_f]*f[idx_f] + sum2[offset_k + (j-1)*sx + i];
                    }
                    else if(i==0 && j==0 && k>0)
                    {
                        sum1[idx] = f[idx_f] + sum1[(k-1)*sx*sy + j*sx + i];
                        sum2[idx] = f[idx_f]*f[idx_f] + sum2[(k-1)*sx*sy + j*sx + i];
                    }
                    else if(i>0 && j>0 && k==0)
                    {
                        sum1[idx] = f[idx_f] + sum1[offset_j + i-1] + sum1[offset_k + (j-1)*sx + i] - sum1[offset_k + (j-1)*sx + i-1];
                        sum2[idx] = f[idx_f]*f[idx_f] + sum2[offset_j + i-1] + sum2[offset_k + (j-1)*sx + i] - sum2[offset_k + (j-1)*sx + i-1];
                    }
                    else if(i>0 && j==0 && k>0)
                    {
                        sum1[idx] = f[idx_f] + sum1[offset_j + i-1] + sum1[(k-1)*sx*sy + j*sx + i] - sum1[(k-1)*sx*sy + j*sx + i-1];
                        sum2[idx] = f[idx_f]*f[idx_f] + sum2[offset_j + i-1] + sum2[(k-1)*sx*sy + j*sx + i] - sum2[(k-1)*sx*sy + j*sx + i-1];
                    }
                    else if(i==0 && j>0 && k>0)
                    {
                        sum1[idx] = f[idx_f] + sum1[offset_k + (j-1)*sx + i]  + sum1[(k-1)*sx*sy + j*sx + i] - sum1[(k-1)*sx*sy + (j-1)*sx + i];
                        sum2[idx] = f[idx_f]*f[idx_f] + sum2[offset_k + (j-1)*sx + i] + sum2[(k-1)*sx*sy + j*sx + i] - sum2[(k-1)*sx*sy + (j-1)*sx + i];
                    }
                    else
                    {
                        sum1[idx] = f[idx_f] + sum1[offset_j + i-1] + sum1[offset_k + (j-1)*sx + i]  + sum1[(k-1)*sx*sy + j*sx + i] + sum1[(k-1)*sx*sy + (j-1)*sx + i-1]
                                - sum1[offset_k + (j-1)*sx + i-1] - sum1[(k-1)*sx*sy + j*sx + i-1] - sum1[(k-1)*sx*sy + (j-1)*sx + i];
                        sum2[idx] = f[idx_f]*f[idx_f] + sum2[offset_j + i-1] + sum2[offset_k + (j-1)*sx + i] + sum2[(k-1)*sx*sy + j*sx + i] + sum2[(k-1)*sx*sy + (j-1)*sx + i-1]
                                - sum2[offset_k + (j-1)*sx + i-1] - sum2[(k-1)*sx*sy + j*sx + i-1] - sum2[(k-1)*sx*sy + (j-1)*sx + i];
                    }
                }
            }
        }
    }
    // init ND sum tables
    void initSumTableND(T2 *sz, T1 *f)
    {
    }
public:	
    T1 *sum1, *sum2;
};
// generate a mst for group tiled images
// T1 V3DLONG T2 REAL
template <class T1, class T2>
int mstPrim(vector<indexed_t<T1, T2> > &tilesList)
{
    //
    T1 size = tilesList.size();
    if(size<1)	return -1;
    else if(size==1) return 0;
    else if(size==2)
    {
        //step 1. choose 0 as anchor image
        (&tilesList.at(0))->predecessor = -1;
        (&tilesList.at(0))->visited = true;
        //step 2. adjust 1's offsets
        (&tilesList.at(1))->predecessor = 0;
        (&tilesList.at(1))->visited = true;
        (&tilesList.at(1))->offsets[0] = tilesList.at(1).record.at(0).offsets[0];
        (&tilesList.at(1))->offsets[1] = tilesList.at(1).record.at(0).offsets[1];
        (&tilesList.at(1))->offsets[2] = tilesList.at(1).record.at(0).offsets[2];
    }
    else
    {
        // Prim's algorithm
        //-------------------------------------------------------------
        //1. Start with a tree which contains only one node.
        //2. Identify a node (outside the tree) which is closest to the tree and add the minimum weight edge from that node to some node in the tree and incorporate the additional node as a part of the tree.
        //3. If there are less then n – 1 edges in the tree, go to 2
        //
        //step 1. choose 0 as anchor image
        (&tilesList.at(0))->predecessor = -1;
        (&tilesList.at(0))->visited = true;
        //step 2.
        //func extractHighestScoreEdge()
        //step 3.
        while(ks(tilesList))
        {
            T1 ni, n2;
            T2 max_score = 0;
            T1 mse_node; // corresponding maxmum score edge
            T1 parent, previous;
            // step 2.
            for(T1 i=0; i<size; i++)
            {
                //
                if(!tilesList.at(i).visited) continue;
                ni = i;
                for(T1 j=1; j<size; j++)
                {
                    if(tilesList.at(j).visited) continue;
                    T1 n1 = ni;
                    n2 = j;
                    // let n1>n2
                    if(n1<n2)
                    {
                        T1 tmp = n1;
                        n1=n2;
                        n2=tmp;
                    }

                    // find highest score edge
                    if(tilesList.at(n1).record.size()>n2)
                    {
                        if(tilesList.at(n1).record.at(n2).score > max_score)
                        {
                            max_score = tilesList.at(n1).record.at(n2).score;
                            mse_node = j; previous = i; parent = tilesList.at(n1).record.at(n2).n;
                        }
                    }
                } // j
            }// i
            // add new node to mst
            T1 sn1=mse_node, sn2=previous, coef=1;
            if(sn1<sn2)
            {
                T1 tmp = sn1;
                sn1=sn2;
                sn2=tmp;
                coef = -1;
            }
            //
            qDebug()<<"mst: current "<<sn1<<"previous "<<parent<<"score "<<max_score;
            //
            (&tilesList.at(mse_node))->offsets[0] = coef*tilesList.at(sn1).record.at(sn2).offsets[0];
            (&tilesList.at(mse_node))->offsets[1] = coef*tilesList.at(sn1).record.at(sn2).offsets[1];
            (&tilesList.at(mse_node))->offsets[2] = coef*tilesList.at(sn1).record.at(sn2).offsets[2];
            (&tilesList.at(mse_node))->predecessor = parent;
            (&tilesList.at(mse_node))->visited = true;
        }
    }
    return 0;
}
// keep searching judgement function
template <class T1, class T2> 
bool ks(vector<indexed_t<T1, T2> > tilesList)
{
    T1 size = tilesList.size();
    for(T1 i=0; i<size; i++)
    {
        if(!tilesList.at(i).visited)
        {
            return true;
        }
    }
    return false;
}
// YImg
// usually T1 is assigned as double and T2 V3DLONG (unsigned V3DLONG int)
template <class T1, class T2, class Y_IMG1, class Y_IMG2>
class YImg 
{
public:
    YImg(){}
    ~YImg(){}
public:
    //func down sampling
    void down_sampling(Y_IMG1 pOut, Y_IMG2 pIn, T1 *scale);
    //func padding zeros for FFT using FFTW lib
    void padding(Y_IMG1 pOut, Y_IMG2 pIn, bool flag, bool fftwf_in_place, T2 even_odd, T2 dims);
    //func padding zeros 2D
    void padding2D(Y_IMG1 pOut, Y_IMG2 pIn, bool upperleft);
    //func padding zeros 3D
    void padding3D(Y_IMG1 pOut, Y_IMG2 pIn, bool frontupperleft, bool fftwf_in_place, T2 even_odd);
    //func padding zeros ND
    void paddingND(Y_IMG1 pOut, Y_IMG2 pIn, bool flag);
    //func padding zeros 3D using symmetric way
    void padding_mirror_3D(Y_IMG1 pOut, Y_IMG2 pIn, bool frontupperleft, bool fftwf_in_place, T2 even_odd);
    //func fft phase-correlation
    void fftpc3D(Y_IMG1 pOut, Y_IMG2 pIn, T2 even_odd, bool fftwf_in_place, PEAKSLIST *peakList);
    //func all-in-one fft phase-correlation and cross-correlation
    void fftpccc3D(Y_IMG1 pOut, Y_IMG2 pIn, T2 even_odd, bool fftwf_in_place, PEAKSLIST *peakList);
    
    //func fft phase-correlation in subpixel extension
    void fftpcsubspace3D(Y_IMG1 pOut, Y_IMG2 pIn, T2 even_odd, bool fftwf_in_place, rPEAKS *pos);
    //func all-in-one fft phase-correlation and cross-correlation using 1d fft
    void fftpatientpccc3D(Y_IMG1 pOut, Y_IMG2 pIn, T2 even_odd, bool fftwf_in_place, PEAKSLIST *peakList);
    //func fft phase-correlation combined with normalized cross-correlation
    void fftpcncc3D(Y_IMG1 pOut, Y_IMG2 pIn, T2 *sz3d, T2 even_odd, bool fftwf_in_place, T1 overlap_percent, PEAKS *pos);
    //func fft cross-correlation
    void fftcc(Y_IMG1 pOut, Y_IMG2 pIn, T2 even_odd, bool fftwf_in_place, T2 dims);
    //func fft cc 2D
    void fftcc2D(Y_IMG1 pOut, Y_IMG2 pIn, bool fftwf_in_place);
    //func fft cc 3D
    void fftcc3D(Y_IMG1 pOut, Y_IMG2 pIn, T2 even_odd, bool fftwf_in_place);
    //func fft cc ND
    void fftccND(Y_IMG1 pOut, Y_IMG2 pIn, bool fftwf_in_place);
    //func fft normalized cross-correlation
    void fftncc(Y_IMG1 pOut, Y_IMG2 pIn, T2 *sz, T2 even_odd, bool fftwf_in_place, T1 overlap_percent, T2 dims);
    //func fft ncc 2D
    void fftncc2D(Y_IMG1 pOut, Y_IMG2 pIn, T2 *sz2d, T1 overlap_percent);
    //func fft ncc 3D
    void fftncc3D(Y_IMG1 pOut, Y_IMG2 pIn, T2 *sz3d, T2 even_odd, bool fftwf_in_place, T1 overlap_percent);
    //func fft ncc 3D with one peak
    void fftnccp3D(Y_IMG1 pOut, Y_IMG2 pIn, T2 *sz_sub, T2 *sz_tar, T2 even_odd, bool fftwf_in_place, T1 *scale, PEAKS *pos);
    //func fft ncc 3D with one peak without precomputing sum table
    void fftnccpns3D(Y_IMG1 pOut, Y_IMG2 pIn, T2 *sz_sub, T2 *sz_tar, T2 even_odd, bool fftwf_in_place, T1 *scale, PEAKS *pos);
    //func fft ncc ND
    void fftnccND(Y_IMG1 pOut, Y_IMG2 pIn, T2 *sznd, T1 overlap_percent);
    
    //func subpixel translate
    void subpixeltranslate(Y_IMG1 pOut, T2 even_odd, bool fftwf_in_place, rPEAKS *pos);
    //func region growing 3D
    void regiongrow3D(Y_IMG1 pOut, Y_IMG2 pIn, rPEAKSLIST *peakList, T2 ncm);
    //func gaussian filtering 3D
    void gaussianfilter3D(Y_IMG1 pOut, Y_IMG2 pIn, T2 *sz_w);
    // func computing normalized cross correlation
    void cmpt_ncc3D(Y_IMG1 pOut, Y_IMG2 pIn, T2 *sz_sub, T2 *sz_tar, T2 even_odd, bool fftwf_in_place, PEAKS *&pos);
    //compute the power of x
    T1 power( T1 x, T2 n)
    {
        T1 r = 1;
        for ( T2 i = 0; i < n; ++i)
            r *= x;
        return r;
    };
public:
    //Y_IMAGE pIn, pOut;
};
//------------------------------------------------------------------------------------------------------------------
// down sampling
template <class T1, class T2, class Y_IMG1, class Y_IMG2> void YImg<T1, T2, Y_IMG1, Y_IMG2> :: down_sampling(Y_IMG1 pOut, Y_IMG2 pIn, T1 *scale)
{
    T1 scale_x=scale[0], scale_y=scale[1], scale_z=scale[2];
    //temporary pointer
    T2 ssx,ssy,ssz,ssc;
    ssx=pOut.sz[0];
    ssy=pOut.sz[1];
    ssz=pOut.sz[2];
    ssc=pOut.sz[3];
    T2 N,M,P,C;
    N=pIn.sz[0];
    M=pIn.sz[1];
    P=pIn.sz[2];
    C=pIn.sz[3];
    //linear interpolation
    for (T2 k=0;k<ssz;k++)
    {
        T2 kstart=T2(floor(k/scale_z)), kend=kstart+T2(1/scale_z);
        if (kend>P-1) kend = P-1;
        if(P==1) kend=1;
        for (T2 j=0;j<ssy;j++)
        {
            T2 jstart=T2(floor(j/scale_y)), jend=jstart+T2(1/scale_y);
            if (jend>M-1) jend = M-1;
            if(M==1) jend=1;
            for (T2 i=0;i<ssx;i++)
            {
                T2 istart=T2(floor(i/scale_x)), iend=istart+T2(1/scale_x);
                if (iend>N-1) iend = N-1;
                if(N==1) iend=1;
                T2 idx_out = k*ssx*ssy + j*ssx + i;
                T2 sum=0;
                for(T2 kk=kstart; kk<kend; kk++)
                {
                    for(T2 jj=jstart; jj<jend; jj++)
                    {
                        for(T2 ii=istart; ii<iend; ii++)
                        {
                            sum += pIn.pImg[kk*M*N + jj*N + ii];
                        }
                    }
                }
                if(iend==istart)
                    pOut.pImg[idx_out] = pIn.pImg[k*M*N + j*N + istart];
                else if(jend==jstart)
                    pOut.pImg[idx_out] = pIn.pImg[k*M*N + jstart*N + i];
                else if(kend==kstart)
                    pOut.pImg[idx_out] = pIn.pImg[kstart*M*N + j*N + i];
                else
                    pOut.pImg[idx_out] =  sum / ( (kend-kstart)*(jend-jstart)*(iend-istart) );
            }
        }
    }
}
//------------------------------------------------------------------------------------------------------------------
// padding zeros
template <class T1, class T2, class Y_IMG1, class Y_IMG2> void YImg<T1, T2, Y_IMG1, Y_IMG2> :: padding(Y_IMG1 pOut, Y_IMG2 pIn, bool flag, bool fftwf_in_place, T2 even_odd, T2 dims)
{
    if(dims == 2)
        padding2D(pOut, pIn, flag);
    else if (dims == 3)
        padding3D(pOut, pIn, flag, fftwf_in_place, even_odd);
    else if (dims > 3)
        paddingND(pOut, pIn, flag);
    else
    {
        printf("Dimensions should be 2, 3, or any positive integer greater than 3.\n");
        return;
    }
}
// padding zeros 2D
template <class T1, class T2, class Y_IMG1, class Y_IMG2> void YImg<T1, T2, Y_IMG1, Y_IMG2> :: padding2D(Y_IMG1 pOut, Y_IMG2 pIn, bool upperleft)
{
    T2 sx,sy,tx,ty,szx_pad,szy_pad;
    if(upperleft)
    {
        sx=pIn.sz[0];
        sy=pIn.sz[1];
        szx_pad=pOut.sz[0];
        szy_pad=pOut.sz[1];
        tx=szx_pad+1-sx;
        ty=szy_pad+1-sy;
    }
    else
    {
        tx=pIn.sz[0];
        ty=pIn.sz[1];
        szx_pad=pOut.sz[0];
        szy_pad=pOut.sz[1];
        sx=szx_pad+1-tx;
        sy=szy_pad+1-ty;
    }
    if(upperleft)
    {
        for(T2 j=0; j<sy; j++)
        {
            T2 offset_j = j*szx_pad;
            T2 offsets = j*sx;
            for(T2 i=0; i<sx; i++)
            {
                pOut.pImg[offset_j + i] = pIn.pImg[offsets + i];
            }
        }
    }
    else
    {
        for(T2 j=sy-1; j<szy_pad; j++)
        {
            T2 offset_j = j*szx_pad;
            T2 offsets = (j-sy+1)*tx;
            for(T2 i=sx-1; i<szx_pad; i++)
            {
                pOut.pImg[offset_j + i] = pIn.pImg[offsets + i-sx+1];
            }
        }
    }
}
// padding zeros 3D
template <class T1, class T2, class Y_IMG1, class Y_IMG2> void YImg<T1, T2, Y_IMG1, Y_IMG2> :: padding3D(Y_IMG1 pOut, Y_IMG2 pIn, bool frontupperleft, bool fftwf_in_place, T2 even_odd)
{
    // by default padding with zeros
    T2 sx,sy,sz, tx,ty,tz, szx_pad,szy_pad, szz_pad;
    szx_pad=pOut.sz[0];
    szy_pad=pOut.sz[1];
    szz_pad=pOut.sz[2];
    T2 szx_pad_tmp;
    if(fftwf_in_place)
        szx_pad_tmp = szx_pad - (2-even_odd); //2*(szx_pad/2-1); //fftwf_in_place
    else
        szx_pad_tmp = szx_pad;
    if(frontupperleft)
    {
        sx=pIn.sz[0];
        sy=pIn.sz[1];
        sz=pIn.sz[2];
        tx=szx_pad_tmp+1-sx; // fftwf_in_place
        ty=szy_pad+1-sy;
        tz=szz_pad+1-sz;
    }
    else
    {
        tx=pIn.sz[0];
        ty=pIn.sz[1];
        tz=pIn.sz[2];
        sx=szx_pad_tmp+1-tx; // fftwf_in_place
        sy=szy_pad+1-ty;
        sz=szz_pad+1-tz;
    }
    // padding
    //
    if(frontupperleft)
    {
        for(T2 k=0; k<sz; k++)
        {
            T2 offset_k = k*szx_pad*szy_pad;
            T2 offsets_k = k*sx*sy;
            for(T2 j=0; j<sy; j++)
            {
                T2 offset_j = offset_k + j*szx_pad;
                T2 offsets = offsets_k + j*sx;
                for(T2 i=0; i<sx; i++)
                {
                    pOut.pImg[offset_j + i] = pIn.pImg[offsets + i];
                }
            }
        }
    }
    else
    {
        for(T2 k=sz-1; k<szz_pad; k++)
        {
            T2 offset_k = k*szx_pad*szy_pad;
            T2 offsets_k = (k-sz+1)*tx*ty;
            for(T2 j=sy-1; j<szy_pad; j++)
            {
                T2 offset_j = offset_k + j*szx_pad;
                T2 offsets = offsets_k + (j-sy+1)*tx;
                for(T2 i=sx-1; i<szx_pad_tmp; i++) // fftwf_in_place
                {
                    pOut.pImg[offset_j + i] = pIn.pImg[offsets + i-sx+1];
                }
            }
        }
    }
}
// padding zeros ND
template <class T1, class T2, class Y_IMG1, class Y_IMG2> void YImg<T1, T2, Y_IMG1, Y_IMG2> :: paddingND(Y_IMG1 pOut, Y_IMG2 pIn, bool flag)
{
}
// 3D image mirror padding
template <class T1, class T2, class Y_IMG1, class Y_IMG2> void YImg<T1, T2, Y_IMG1, Y_IMG2> :: padding_mirror_3D(Y_IMG1 pOut, Y_IMG2 pIn, bool frontupperleft, bool fftwf_in_place, T2 even_odd)
{
    // using symmetric way
    T2 sx,sy,sz, tx,ty,tz, szx_pad,szy_pad, szz_pad;
    szx_pad=pOut.sz[0];
    szy_pad=pOut.sz[1];
    szz_pad=pOut.sz[2];
    T2 szx_pad_tmp;
    if(fftwf_in_place)
        szx_pad_tmp = szx_pad - (2-even_odd); //2*(szx_pad/2-1); //fftwf_in_place
    else
        szx_pad_tmp = szx_pad;
    if(frontupperleft)
    {
        sx=pIn.sz[0];
        sy=pIn.sz[1];
        sz=pIn.sz[2];
        tx=szx_pad_tmp+1-sx; // fftwf_in_place
        ty=szy_pad+1-sy;
        tz=szz_pad+1-sz;
    }
    else
    {
        tx=pIn.sz[0];
        ty=pIn.sz[1];
        tz=pIn.sz[2];
        sx=szx_pad_tmp+1-tx; // fftwf_in_place
        sy=szy_pad+1-ty;
        sz=szz_pad+1-tz;
    }
    // padding
    V3DLONG x, y, z;
    if(frontupperleft)
    {
        for(T2 k=0; k<sz+8; k++) //szz_pad
        {
            T2 offset_k = k*szx_pad*szy_pad;
            T2 offsets_k = k*sx*sy;
            for(T2 j=0; j<sy+8; j++) //szy_pad
            {
                T2 offset_j = offset_k + j*szx_pad;
                T2 offsets = offsets_k + j*sx;
                for(T2 i=0; i<sx+8; i++) //szx_pad
                {
                    V3DLONG idx = offset_j + i;
                    if(i<sx && j<sy && k<sz)
                    {
                        pOut.pImg[idx] = pIn.pImg[offsets + i];
                    }
                    else
                    {
                        if(i>=sx) x = sx - (i-sx+2); else x = i;
                        if(j>=sy) y = sy - (j-sy+2); else y = j;
                        if(k>=sz) z = sz - (k-sz+2); else z = k;
                        if(x>=0 && y>=0 && z>=0)
                        {
                            pOut.pImg[idx] = pIn.pImg[z*sx*sy + y*sx + x];
                        }
                    }
                }
            }
        }
    }
    else
    {
        for(T2 k=sz-9; k<szz_pad; k++) //
        {
            T2 offset_k = k*szx_pad*szy_pad;
            T2 offsets_k = (k-sz+1)*tx*ty;
            for(T2 j=sy-9; j<szy_pad; j++) //
            {
                T2 offset_j = offset_k + j*szx_pad;
                T2 offsets = offsets_k + (j-sy+1)*tx;
                for(T2 i=sx-9; i<szx_pad_tmp; i++) // fftwf_in_place
                {
                    V3DLONG idx = offset_j + i;
                    x = i-sx+1;
                    y = j-sy+1;
                    z = k-sz+1;
                    if(x>=0 && y>=0 && z>=0)
                    {
                        pOut.pImg[idx] = pIn.pImg[offsets + i-sx+1];
                    }
                    else
                    {
                        if (x < 0)
                        {
                            int tmp = 0;
                            int dir = 1;
                            while (x < 0)
                            {
                                tmp += dir;
                                if (tmp == sx - 1 || tmp == 0) dir *= -1;
                                x++;
                            }
                            x = tmp;
                        }
                        if (y < 0)
                        {
                            int tmp = 0;
                            int dir = 1;
                            while (y < 0)
                            {
                                tmp += dir;
                                if (tmp == sy - 1 || tmp == 0) dir *= -1;
                                y++;
                            }
                            y = tmp;
                        }
                        if (z < 0)
                        {
                            int tmp = 0;
                            int dir = 1;
                            while (z < 0)
                            {
                                tmp += dir;
                                if (tmp == sz - 1 || tmp == 0) dir *= -1;
                                z++;
                            }
                            z = tmp;
                        }
                        if(x>=0 && y>=0 && z>=0)
                        {
                            pOut.pImg[idx] = pIn.pImg[z*tx*ty + y*tx + x];
                        }
                    }
                }
            }
        }
    }
}
//------------------------------------------------------------------------------------------------------------------
// fft phase-correlation
// fft pc 3D
template <class T1, class T2, class Y_IMG1, class Y_IMG2> void YImg<T1, T2, Y_IMG1, Y_IMG2> :: fftpc3D(Y_IMG1 pOut, Y_IMG2 pIn, T2 even_odd, bool fftwf_in_place, PEAKSLIST *peakList)
{
    // peaks
    PEAKS pos;
    T1 max_v=0;
    T2 sx_pad, sy_pad, sz_pad;
    sx_pad=pIn.sz[0];
    sy_pad=pIn.sz[1];
    sz_pad=pIn.sz[2];
    T2 len_pad = sx_pad*sy_pad*sz_pad;
    //
    //fftwf_init_threads(); //pthread
    //fftwf_plan_with_nthreads(PROCESSORS);
    fftwf_plan p;
    if(fftwf_in_place)
    {
        T2 sx_pad_ori = sx_pad - (2-even_odd); //2*(sx_pad/2-1);
        T2 len_pad_tmp = sx_pad_ori*sy_pad*sz_pad;
        p = fftwf_plan_dft_r2c_3d(sz_pad, sy_pad, sx_pad_ori, pIn.pImg, (fftwf_complex*)pIn.pImg, FFTW_ESTIMATE);
        fftwf_execute(p);
        fftwf_destroy_plan(p);
        p = fftwf_plan_dft_r2c_3d(sz_pad, sy_pad, sx_pad_ori, pOut.pImg, (fftwf_complex*)pOut.pImg, FFTW_ESTIMATE);
        fftwf_execute(p);
        fftwf_destroy_plan(p);
        // obtain the cross power spectrum
        for(T2 k=0; k<sz_pad; k++)
        {
            T2 offset_k = k*sy_pad*sx_pad;
            for(T2 j=0; j<sy_pad; j++)
            {
                T2 offset_j = offset_k + j*sx_pad;
                for(T2 i=0; i<sx_pad; i+=2)
                {
                    T2 idx = offset_j + i;
                    T1 tmp = pOut.pImg[idx];
                    pOut.pImg[idx+1] = -pOut.pImg[idx+1]; //conjugate
                    pOut.pImg[idx] = pIn.pImg[idx]*tmp - pIn.pImg[idx+1]*pOut.pImg[idx+1];
                    pOut.pImg[idx+1] = pIn.pImg[idx+1]*tmp + pIn.pImg[idx]*pOut.pImg[idx+1];
                    T1 tmp2 = sqrt(power(pOut.pImg[idx], 2) + power(pOut.pImg[idx+1], 2));
                    pOut.pImg[idx] /= tmp2;
                    pOut.pImg[idx+1] /= tmp2;
                }
            }
        }
        // obtain the phase correlation
        fftwf_complex* out_sub = (fftwf_complex*)pOut.pImg;
        p = fftwf_plan_dft_c2r_3d(sz_pad, sy_pad, sx_pad_ori, out_sub, pOut.pImg, FFTW_ESTIMATE);
        fftwf_execute(p);
        fftwf_destroy_plan(p);
        // normalize
        for(T2 k=0; k<sz_pad; k++)
        {
            T2 offset_k = k*sy_pad*sx_pad;
            for(T2 j=0; j<sy_pad; j++)
            {
                T2 offset_j = offset_k + j*sx_pad;
                for(T2 i=0; i<sx_pad; i++)
                {
                    T2 idx = offset_j + i;
                    if(i<sx_pad_ori)
                    {
                        pOut.pImg[idx] /= (T1)len_pad_tmp; //
                    }
                    else
                        pOut.pImg[idx] = 0;
                    // statistics of peaks
                    if(pOut.pImg[idx] > max_v)
                    {
                        max_v = pOut.pImg[idx];
                        pos.x = i; pos.y = j; pos.z = k; pos.value = max_v;
                        if(peakList->size()<1)
                            peakList->push_back(pos);
                        else
                        {
                            for(unsigned int it=peakList->size(); it!=0; it--)
                            {
                                if(pos.value>=peakList->at(it-1).value)
                                {
                                    peakList->insert(peakList->begin() + it, 1, pos);
                                    if(peakList->size()>NPEAKS)
                                        peakList->erase(peakList->begin());
                                    break;
                                }
                                else
                                    continue;
                            }
                            if(pos.value<peakList->at(0).value && peakList->size()<NPEAKS)
                                peakList->insert(peakList->begin(), pos);
                        }
                    }
                }
            }
        }
    }
    else
    {
        T2 fsx = sx_pad/2+1;
        T2 out_dims = sz_pad*sy_pad*fsx;
        fftwf_complex* out_sub;
        fftwf_complex* out_tar;
        out_sub = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * out_dims);
        out_tar = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * out_dims);
        p = fftwf_plan_dft_r2c_3d(sz_pad, sy_pad, sx_pad, pIn.pImg, out_tar, FFTW_ESTIMATE);
        fftwf_execute(p);
        fftwf_destroy_plan(p);
        p = fftwf_plan_dft_r2c_3d(sz_pad, sy_pad, sx_pad, pOut.pImg, out_sub, FFTW_ESTIMATE);
        fftwf_execute(p);
        fftwf_destroy_plan(p);
        // obtain the cross power spectrum
        for(T2 k=0; k<sz_pad; k++)
        {
            T2 offset_k = k*sy_pad*fsx;
            for(T2 j=0; j<sy_pad; j++)
            {
                T2 offset_j = offset_k + j*fsx;
                for(T2 i=0; i<fsx; i++)
                {
                    T2 idx = offset_j + i;
                    //out_sub as output
                    T1 tmp = out_sub[idx][0];
                    out_sub[idx][1] = -out_sub[idx][1]; //conjugate
                    out_sub[idx][0] = out_tar[idx][0]*tmp - out_tar[idx][1]*out_sub[idx][1];
                    out_sub[idx][1] = out_tar[idx][1]*tmp + out_tar[idx][0]*out_sub[idx][1];
                    T1 tmp2 = sqrt(power(out_sub[idx][0], 2) + power(out_sub[idx][1], 2));
                    out_sub[idx][0] /= tmp2;
                    out_sub[idx][1] /= tmp2;
                }
            }
        }
        // obtain the phase correlation
        for(T2 i=0; i<len_pad; i++)
            pOut.pImg[i] = 0;
        p = fftwf_plan_dft_c2r_3d(sz_pad, sy_pad, sx_pad, out_sub, pOut.pImg, FFTW_ESTIMATE);
        fftwf_execute(p);
        fftwf_destroy_plan(p);
        // normalize
        for(T2 k=0; k<sz_pad; k++)
        {
            T2 offset_k = k*sy_pad*sx_pad;
            for(T2 j=0; j<sy_pad; j++)
            {
                T2 offset_j = offset_k + j*sx_pad;
                for(T2 i=0; i<sx_pad; i++)
                {
                    T2 idx = offset_j + i;
                    pOut.pImg[idx] /= (T1)len_pad;
                    // statistics of peaks
                    if(pOut.pImg[idx] > max_v)
                    {
                        max_v = pOut.pImg[idx];
                        pos.x = i; pos.y = j; pos.z = k; pos.value = max_v;
                        if(peakList->size()<1)
                            peakList->push_back(pos);
                        else
                        {
                            for(unsigned int it=peakList->size(); it!=0; it--)
                            {
                                if(pos.value>=peakList->at(it-1).value)
                                {
                                    peakList->insert(peakList->begin() + it, 1, pos);
                                    if(peakList->size()>NPEAKS)
                                        peakList->erase(peakList->begin());
                                    break;
                                }
                                else
                                    continue;
                            }
                            if(pos.value<peakList->at(0).value && peakList->size()<NPEAKS)
                                peakList->insert(peakList->begin(), pos);
                        }
                    }
                }
            }
        }
        //de-alloc
        fftwf_free(out_sub);
        fftwf_free(out_tar);
    }
    // cleanup
    // fftwf_cleanup_threads();
}
//------------------------------------------------------------------------------------------------------------------
// fft cross-correlation
template <class T1, class T2, class Y_IMG1, class Y_IMG2> void YImg<T1, T2, Y_IMG1, Y_IMG2> :: fftcc(Y_IMG1 pOut, Y_IMG2 pIn, T2 even_odd, bool fftwf_in_place, T2 dims)
{
    if(dims == 2)
        fftcc2D(pOut, pIn, fftwf_in_place);
    else if (dims == 3)
        fftcc3D(pOut, pIn, even_odd, fftwf_in_place);
    else if (dims > 3)
        fftccND(pOut, pIn, fftwf_in_place);
    else
    {
        printf("Dimensions should be 2, 3, or any positive integer greater than 3.\n");
        return;
    }
}
// fft cc 2D
template <class T1, class T2, class Y_IMG1, class Y_IMG2> void YImg<T1, T2, Y_IMG1, Y_IMG2> :: fftcc2D(Y_IMG1 pOut, Y_IMG2 pIn, bool fftwf_in_place)
{
    //assuming the pIn and pOut already padded by zeros
    //subject is on the top left (do conjugate in the frequancy) and target is on the bottom right
    //pOut is subject pIn is target with same size
    // fftwf_out_place
    T2 sx_pad, sy_pad;
    sx_pad=pIn.sz[0];
    sy_pad=pIn.sz[1];
    T2 len_pad = sx_pad*sy_pad;
    T2 fsx = sx_pad/2+1;
    T2 out_dims = sy_pad*fsx;
    fftwf_complex* out_sub;
    fftwf_complex* out_tar;
    out_sub = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * out_dims);
    out_tar = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * out_dims);
    //fftwf_init_threads(); //pthread
    //fftwf_plan_with_nthreads(8);
    fftwf_plan p;
    p = fftwf_plan_dft_r2c_2d(sy_pad, sx_pad, pIn.pImg, out_tar, FFTW_ESTIMATE);
    fftwf_execute(p);
    fftwf_destroy_plan(p);
    p = fftwf_plan_dft_r2c_2d(sy_pad, sx_pad, pOut.pImg, out_sub, FFTW_ESTIMATE);
    fftwf_execute(p);
    fftwf_destroy_plan(p);
    //fft cc
    for(T2 j=0; j<sy_pad; j++)
    {
        T2 offset_j = j*fsx;
        for(T2 i=0; i<fsx; i++)
        {
            T2 idx = offset_j + i;
            //out_sub as output
            T1 tmp = out_sub[idx][0];
            out_sub[idx][1] = -out_sub[idx][1]; //conjugate
            out_sub[idx][0] = out_tar[idx][0]*tmp - out_tar[idx][1]*out_sub[idx][1];
            out_sub[idx][1] = out_tar[idx][1]*tmp + out_tar[idx][0]*out_sub[idx][1];
        }
    }
    for(T2 i=0; i<len_pad; i++)
        pOut.pImg[i] = 0;
    p = fftwf_plan_dft_c2r_2d(sy_pad, sx_pad, out_sub, pOut.pImg, FFTW_ESTIMATE);
    fftwf_execute(p);
    fftwf_destroy_plan(p);
    for(T2 i=0; i<len_pad; i++)
    {
        pOut.pImg[i] /= len_pad;
    }
    //de-alloc
    fftwf_free(out_sub);
    fftwf_free(out_tar);
    // cleanup
    //fftwf_cleanup_threads();
}
// fft cc 3D
template <class T1, class T2, class Y_IMG1, class Y_IMG2> void YImg<T1, T2, Y_IMG1, Y_IMG2> :: fftcc3D(Y_IMG1 pOut, Y_IMG2 pIn, T2 even_odd, bool fftwf_in_place)
{
    T2 sx_pad, sy_pad, sz_pad;
    sx_pad=pIn.sz[0];
    sy_pad=pIn.sz[1];
    sz_pad=pIn.sz[2];
    T2 len_pad = sx_pad*sy_pad*sz_pad;
    //
    //fftwf_init_threads(); //pthread
    //fftwf_plan_with_nthreads(PROCESSORS);
    fftwf_plan p;
    if(fftwf_in_place)
    {
        T2 sx_pad_ori = sx_pad - (2-even_odd); //2*(sx_pad/2-1);
        T2 len_pad_tmp = sx_pad_ori*sy_pad*sz_pad;
        p = fftwf_plan_dft_r2c_3d(sz_pad, sy_pad, sx_pad_ori, pIn.pImg, (fftwf_complex*)pIn.pImg, FFTW_ESTIMATE);
        fftwf_execute(p);
        fftwf_destroy_plan(p);
        p = fftwf_plan_dft_r2c_3d(sz_pad, sy_pad, sx_pad_ori, pOut.pImg, (fftwf_complex*)pOut.pImg, FFTW_ESTIMATE);
        fftwf_execute(p);
        fftwf_destroy_plan(p);
        for(T2 k=0; k<sz_pad; k++)
        {
            T2 offset_k = k*sy_pad*sx_pad;
            for(T2 j=0; j<sy_pad; j++)
            {
                T2 offset_j = offset_k + j*sx_pad;
                for(T2 i=0; i<sx_pad; i+=2)
                {
                    T2 idx = offset_j + i;
                    T1 tmp = pOut.pImg[idx];
                    pOut.pImg[idx+1] = -pOut.pImg[idx+1]; //conjugate
                    pOut.pImg[idx] = pIn.pImg[idx]*tmp - pIn.pImg[idx+1]*pOut.pImg[idx+1];
                    pOut.pImg[idx+1] = pIn.pImg[idx+1]*tmp + pIn.pImg[idx]*pOut.pImg[idx+1];
                }
            }
        }
        fftwf_complex* out_sub = (fftwf_complex*)pOut.pImg;
        //fftwf_complex* out_tar = (fftwf_complex*)pIn.pImg;
        p = fftwf_plan_dft_c2r_3d(sz_pad, sy_pad, sx_pad_ori, out_sub, pOut.pImg, FFTW_ESTIMATE);
        fftwf_execute(p);
        fftwf_destroy_plan(p);
        for(T2 k=0; k<sz_pad; k++)
        {
            T2 offset_k = k*sy_pad*sx_pad;
            for(T2 j=0; j<sy_pad; j++)
            {
                T2 offset_j = offset_k + j*sx_pad;
                for(T2 i=0; i<sx_pad; i++)
                {
                    T2 idx = offset_j + i;
                    if(i<sx_pad_ori)
                    {
                        pOut.pImg[idx] /= (T1)len_pad_tmp; //
                    }
                    else
                        pOut.pImg[idx] = 0;
                }
            }
        }

    }
    else
    {
        T2 fsx = sx_pad/2+1;
        T2 out_dims = sz_pad*sy_pad*fsx;
        fftwf_complex* out_sub;
        fftwf_complex* out_tar;
        out_sub = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * out_dims);
        out_tar = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * out_dims);
        p = fftwf_plan_dft_r2c_3d(sz_pad, sy_pad, sx_pad, pIn.pImg, out_tar, FFTW_ESTIMATE);

        fftwf_execute(p);
        fftwf_destroy_plan(p);
        p = fftwf_plan_dft_r2c_3d(sz_pad, sy_pad, sx_pad, pOut.pImg, out_sub, FFTW_ESTIMATE);
        fftwf_execute(p);
        fftwf_destroy_plan(p);
        //fft cc
        for(T2 k=0; k<sz_pad; k++)
        {
            T2 offset_k = k*sy_pad*fsx;
            for(T2 j=0; j<sy_pad; j++)
            {
                T2 offset_j = offset_k + j*fsx;
                for(T2 i=0; i<fsx; i++)
                {
                    T2 idx = offset_j + i;
                    //out_sub as output
                    T1 tmp = out_sub[idx][0];
                    out_sub[idx][1] = -out_sub[idx][1]; //conjugate
                    out_sub[idx][0] = out_tar[idx][0]*tmp - out_tar[idx][1]*out_sub[idx][1];
                    out_sub[idx][1] = out_tar[idx][1]*tmp + out_tar[idx][0]*out_sub[idx][1];
                }
            }
        }
        for(T2 i=0; i<len_pad; i++)
            pOut.pImg[i] = 0;
        p = fftwf_plan_dft_c2r_3d(sz_pad, sy_pad, sx_pad, out_sub, pOut.pImg, FFTW_ESTIMATE);
        fftwf_execute(p);
        fftwf_destroy_plan(p);
        for(T2 i=0; i<len_pad; i++)
        {
            pOut.pImg[i] /= (T1)len_pad;
        }
        //de-alloc
        fftwf_free(out_sub);
        fftwf_free(out_tar);
    }
    // cleanup
    //fftwf_cleanup_threads();
}
// fft cc ND
template <class T1, class T2, class Y_IMG1, class Y_IMG2> void YImg<T1, T2, Y_IMG1, Y_IMG2> :: fftccND(Y_IMG1 pOut, Y_IMG2 pIn, bool fftwf_in_place)
{
}
// fft normalized cross-correlation
template <class T1, class T2, class Y_IMG1, class Y_IMG2> void YImg<T1, T2, Y_IMG1, Y_IMG2> :: fftncc(Y_IMG1 pOut, Y_IMG2 pIn, T2 *sz, T2 even_odd, bool fftwf_in_place, T1 overlap_percent, T2 dims)
{
    if(dims == 2)
        fftncc2D(pOut, pIn, sz, overlap_percent);
    else if (dims == 3)
        fftncc3D(pOut, pIn, sz, even_odd, fftwf_in_place, overlap_percent);
    else if (dims > 3)
        fftnccND(pOut, pIn, sz, overlap_percent);
    else
    {
        printf("Dimensions should be 2, 3, or any positive integer greater than 3.\n");
        return;
    }
}
// fft ncc 2D
template <class T1, class T2, class Y_IMG1, class Y_IMG2> void YImg<T1, T2, Y_IMG1, Y_IMG2> :: fftncc2D(Y_IMG1 pOut, Y_IMG2 pIn, T2 *sz2d, T1 overlap_percent)
{
    //assuming the pIn and pOut already padded by zeros
    //subject is on the top left (do conjugate in the frequancy) and target is on the bottom right
    T2 tx=sz2d[0], ty=sz2d[1];
    T2 sx_pad, sy_pad;
    sx_pad=pIn.sz[0];
    sy_pad=pIn.sz[1];
    T2 len_pad = sx_pad*sy_pad;
    T2 sx, sy;
    sx=sx_pad+1-tx;
    sy=sy_pad+1-ty;
    T2 sub_sz = sx*sy;
    NST<T1, T2> ncctar(pIn.sz, pIn.pImg, 0, false, 2);
    NST<T1, T2> nccsub(pOut.sz, pOut.pImg, 0, false, 2);
    //cross-correlation
    fftcc2D(pOut, pIn, false);
    //
    for(T2 v=0; v<sy_pad; v++)
    {
        T2 offset_v = v*sx_pad;
        for(T2 u=0; u<sx_pad; u++)
        {
            T2 idx = offset_v + u;
            T2 t_lu_idx, t_ru_idx, t_ld_idx, t_rd_idx;
            T2 s_lu_idx, s_ru_idx, s_ld_idx, s_rd_idx;
            T2 t_lx, t_rx, t_uy, t_dy;
            T2 s_lx, s_rx, s_uy, s_dy;
            T1 t_std, t_mean;
            T1 s_std, s_mean;
            if(u<sx && u<tx)
            {
                s_lx = sx-1 - u; s_rx = sx-1;
                t_lx = sx-1; t_rx = sx-1 + u;
            }
            else if(u<sx && u>=tx)
            {
                s_lx = sx-1 - u; s_rx = sx_pad-1 - u;
                t_lx = sx-1; t_rx = sx_pad-1;
            }
            else if(u>=sx && u<tx)
            {
                s_lx = 0; s_rx = sx-1;
                t_lx = u; t_rx = sx-1 + u;
            }
            else if(u>=tx)
            {
                s_lx = 0; s_rx = sx_pad-1 - u;
                t_lx = u; t_rx = sx_pad-1;
            }
            else
                printf("x direction %ld %ld %ld %ld \n", u, sx, tx, sx_pad);
            if(v<sy && v<ty)
            {
                s_uy = sy-1 - v; s_dy = sy-1;
                t_uy = sy-1; t_dy = sy-1 + v;
            }
            else if(v<sy && v>=ty)
            {
                s_uy = sy-1 - v; s_dy = sy_pad-1 - v;
                t_uy = sy-1; t_dy = sy_pad-1;
            }
            else if(v<ty && v>=sy)
            {
                s_uy = 0; s_dy = sy-1;
                t_uy = v; t_dy = sy-1 + v;
            }
            else if(v>=ty)
            {
                s_uy = 0; s_dy = sy_pad-1 - v;
                t_uy = v; t_dy = sy_pad-1;
            }
            else
                printf("y direction %ld %ld %ld %ld \n", v, sy, ty, sy_pad);
            if(t_rx<t_lx || t_dy<t_uy || s_rx<s_lx || s_dy<s_uy)
            {
                pOut.pImg[idx] = 0;
                continue;
            }
            t_lu_idx = (t_uy-1)*sx_pad + t_lx-1;
            t_ru_idx = (t_uy-1)*sx_pad + t_rx;
            t_ld_idx = t_dy*sx_pad + t_lx-1;
            t_rd_idx = t_dy*sx_pad + t_rx;
            s_lu_idx = (s_uy-1)*sx_pad + s_lx-1;
            s_ru_idx = (s_uy-1)*sx_pad + s_rx;
            s_ld_idx = s_dy*sx_pad + s_lx-1;
            s_rd_idx = s_dy*sx_pad + s_rx;
            T2 len_t = (t_dy - t_uy + 1)*(t_rx - t_lx + 1), len_s = (s_dy - s_uy + 1)*(s_rx - s_lx + 1);
            // prior knowledge of ratio of overlap
            if(len_s<sub_sz*overlap_percent)
            {
                pOut.pImg[idx] = 0;
                continue;
            }
            T1 t1,t2;
            if(t_lx==0 && t_uy==0)
            {
                t1 = ncctar.sum1[t_rd_idx];
                t2 = ncctar.sum2[t_rd_idx];
            }
            else if(t_lx==0 && t_uy>0)
            {
                t1 = ncctar.sum1[t_rd_idx] - ncctar.sum1[t_ru_idx];
                t2 = ncctar.sum2[t_rd_idx] - ncctar.sum2[t_ru_idx];
            }
            else if(t_lx>0 && t_uy==0)
            {
                t1 = ncctar.sum1[t_rd_idx] - ncctar.sum1[t_ld_idx];
                t2 = ncctar.sum2[t_rd_idx] - ncctar.sum2[t_ld_idx];
            }
            else
            {
                t1 = fabs( ncctar.sum1[t_rd_idx] + ncctar.sum1[t_lu_idx] - ncctar.sum1[t_ld_idx] - ncctar.sum1[t_ru_idx] );
                t2 = fabs( ncctar.sum2[t_rd_idx] + ncctar.sum2[t_lu_idx] - ncctar.sum2[t_ld_idx] - ncctar.sum2[t_ru_idx] );
            }
            t_mean = t1/len_t;
            t_std = sqrt(y_max(t2 - t1*t_mean, T1(0) ));
            T1 s1,s2;
            if(s_lx==0 && s_uy==0)
            {
                s1 = nccsub.sum1[s_rd_idx];
                s2 = nccsub.sum2[s_rd_idx];
            }
            else if(s_lx==0 && s_uy>0)
            {
                s1 = nccsub.sum1[s_rd_idx] - nccsub.sum1[s_ru_idx];
                s2 = nccsub.sum2[s_rd_idx] - nccsub.sum2[s_ru_idx];
            }
            else if(s_lx>0 && s_uy==0)
            {
                s1 = nccsub.sum1[s_rd_idx] - nccsub.sum1[s_ld_idx];
                s2 = nccsub.sum2[s_rd_idx] - nccsub.sum2[s_ld_idx];
            }
            else
            {
                s1 = fabs( nccsub.sum1[s_rd_idx] + nccsub.sum1[s_lu_idx] - nccsub.sum1[s_ld_idx] - nccsub.sum1[s_ru_idx] );
                s2 = fabs( nccsub.sum2[s_rd_idx] + nccsub.sum2[s_lu_idx] - nccsub.sum2[s_ld_idx] - nccsub.sum2[s_ru_idx] );
            }
            s_mean = s1/len_s;
            s_std = sqrt(y_max(s2 - s1*s_mean, T1(0) ));
            // response [0, 1] (instead of [-1, 1])
            if(t_std!=0 && s_std!=0)
            {
                pOut.pImg[idx] = 0.5 + 0.5*(pOut.pImg[idx] - t1*s_mean)/(t_std*s_std);
            }
            else
            {
                pOut.pImg[idx] = 0;
            }
        }
    }
}
// fft ncc 3D
template <class T1, class T2, class Y_IMG1, class Y_IMG2> void YImg<T1, T2, Y_IMG1, Y_IMG2> :: fftncc3D(Y_IMG1 pOut, Y_IMG2 pIn, T2 *sz3d, T2 even_odd, bool fftwf_in_place, T1 overlap_percent)
{
    T2 tx=sz3d[0], ty=sz3d[1], tz=sz3d[2];
    T2 sx_pad, sy_pad, sz_pad;
    sx_pad=pIn.sz[0];
    sy_pad=pIn.sz[1];
    sz_pad=pIn.sz[2];
    T2 len_pad = sx_pad*sy_pad*sz_pad;
    T2 sx, sy, sz;
    T2 sx_pad_ori;
    if(fftwf_in_place)
        sx_pad_ori = sx_pad - (2-even_odd); //2*(sx_pad/2-1);
    else
        sx_pad_ori = sx_pad;
    sx=sx_pad_ori+1-tx; // fftwf_in_place
    sy=sy_pad+1-ty;
    sz=sz_pad+1-tz;
    T2 sub_sz = sx*sy*sz;
    NST<T1, T2> ncctar(pIn.sz, pIn.pImg, even_odd, fftwf_in_place, 3);
    NST<T1, T2> nccsub(pOut.sz, pOut.pImg, even_odd, fftwf_in_place, 3);
    //cross-correlation
    fftcc3D(pOut, pIn, even_odd, fftwf_in_place);
    //
    for(T2 w=0; w<sz_pad; w++)
    {
        T2 offset_w = w*sx_pad*sy_pad;
        for(T2 v=0; v<sy_pad; v++)
        {
            T2 offset_v = offset_w + v*sx_pad;
            for(T2 u=0; u<sx_pad_ori; u++) // fftwf_in_place
            {
                T2 idx = offset_v + u;
                // z - (front, back) y - (upper, down) x - (left, right)
                T2 t_ful_idx, t_fur_idx, t_fdl_idx, t_fdr_idx, t_bul_idx, t_bur_idx, t_bdl_idx, t_bdr_idx;
                T2 s_ful_idx, s_fur_idx, s_fdl_idx, s_fdr_idx, s_bul_idx, s_bur_idx, s_bdl_idx, s_bdr_idx;
                T2 t_lx, t_rx, t_uy, t_dy, t_fz, t_bz;
                T2 s_lx, s_rx, s_uy, s_dy, s_fz, s_bz;
                T1 t_std, t_mean;
                T1 s_std, s_mean;
                if(u<sx && u<tx)
                {
                    s_lx = sx-1 - u; s_rx = sx-1;
                    t_lx = sx-1; t_rx = sx-1 + u;
                }
                else if(u<sx && u>=tx)
                {
                    s_lx = sx-1 - u; s_rx = sx_pad_ori-1 - u;
                    t_lx = sx-1; t_rx = sx_pad_ori-1;
                }
                else if(u>=sx && u<tx)
                {
                    s_lx = 0; s_rx = sx-1;
                    t_lx = u; t_rx = sx-1 + u;
                }
                else if(u>=tx)
                {
                    s_lx = 0; s_rx = sx_pad_ori-1 - u;
                    t_lx = u; t_rx = sx_pad_ori-1;
                }
                else
                    printf("x direction %ld %ld %ld %ld \n", u, sx, tx, sx_pad);
                if(v<sy && v<ty)
                {
                    s_uy = sy-1 - v; s_dy = sy-1;
                    t_uy = sy-1; t_dy = sy-1 + v;
                }
                else if(v<sy && v>=ty)
                {
                    s_uy = sy-1 - v; s_dy = sy_pad-1 - v;
                    t_uy = sy-1; t_dy = sy_pad-1;
                }
                else if(v<ty && v>=sy)
                {
                    s_uy = 0; s_dy = sy-1;
                    t_uy = v; t_dy = sy-1 + v;
                }
                else if(v>=ty)
                {
                    s_uy = 0; s_dy = sy_pad-1 - v;
                    t_uy = v; t_dy = sy_pad-1;
                }
                else
                    printf("y direction %ld %ld %ld %ld \n", v, sy, ty, sy_pad);
                if(w<sz && w<tz)
                {
                    s_fz = sz-1 - w; s_bz = sz-1;
                    t_fz = sz-1; t_bz = sz-1 + w;
                }
                else if(w<sz && w>=tz)
                {
                    s_fz = sz-1 - w; s_bz = sz_pad-1 - w;
                    t_fz = sz-1; t_bz = sz_pad-1;
                }
                else if(w<tz && w>=sz)
                {
                    s_fz = 0; s_bz = sz-1;
                    t_fz = w; t_bz = sz-1 + w;
                }
                else if(w>=tz)
                {
                    s_fz = 0; s_bz = sz_pad-1 - w;
                    t_fz = w; t_bz = sz_pad-1;
                }
                else
                    printf("z direction %ld %ld %ld %ld \n", w, sz, tz, sz_pad);
                if(t_rx<t_lx || t_dy<t_uy || t_bz<t_fz || s_rx<s_lx || s_dy<s_uy || s_bz<s_fz)
                {
                    pOut.pImg[idx] = 0;
                    continue;
                }
                // overlap boundary
                t_ful_idx = (t_fz-1)*sx_pad_ori*sy_pad + (t_uy-1)*sx_pad_ori + t_lx-1;
                t_fur_idx = (t_fz-1)*sx_pad_ori*sy_pad + (t_uy-1)*sx_pad_ori + t_rx;
                t_fdl_idx = (t_fz-1)*sx_pad_ori*sy_pad + t_dy*sx_pad_ori + t_lx-1;
                t_fdr_idx = (t_fz-1)*sx_pad_ori*sy_pad + t_dy*sx_pad_ori + t_rx;
                t_bul_idx = t_bz*sx_pad_ori*sy_pad + (t_uy-1)*sx_pad_ori + t_lx-1;
                t_bur_idx = t_bz*sx_pad_ori*sy_pad + (t_uy-1)*sx_pad_ori + t_rx;
                t_bdl_idx = t_bz*sx_pad_ori*sy_pad + t_dy*sx_pad_ori + t_lx-1;
                t_bdr_idx = t_bz*sx_pad_ori*sy_pad + t_dy*sx_pad_ori + t_rx;
                s_ful_idx = (s_fz-1)*sx_pad_ori*sy_pad + (s_uy-1)*sx_pad_ori + s_lx-1;
                s_fur_idx = (s_fz-1)*sx_pad_ori*sy_pad + (s_uy-1)*sx_pad_ori + s_rx;
                s_fdl_idx = (s_fz-1)*sx_pad_ori*sy_pad + s_dy*sx_pad_ori + s_lx-1;
                s_fdr_idx = (s_fz-1)*sx_pad_ori*sy_pad + s_dy*sx_pad_ori + s_rx;
                s_bul_idx = s_bz*sx_pad_ori*sy_pad + (s_uy-1)*sx_pad_ori + s_lx-1;
                s_bur_idx = s_bz*sx_pad_ori*sy_pad + (s_uy-1)*sx_pad_ori + s_rx;
                s_bdl_idx = s_bz*sx_pad_ori*sy_pad + s_dy*sx_pad_ori + s_lx-1;
                s_bdr_idx = s_bz*sx_pad_ori*sy_pad + s_dy*sx_pad_ori + s_rx;
                T2 len_t = (t_bz - t_fz + 1)*(t_dy - t_uy + 1)*(t_rx - t_lx + 1), len_s = (s_bz - s_fz + 1)*(s_dy - s_uy + 1)*(s_rx - s_lx + 1);
                // prior knowledge of ratio of overlap
                if(T1(len_s)<T1(sub_sz)*overlap_percent)
                {
                    pOut.pImg[idx] = 0;
                    continue;
                }
                T1 t1,t2;
                if(t_lx==0 && t_uy==0 && t_fz==0)
                {
                    t1 = ncctar.sum1[t_bdr_idx];
                    t2 = ncctar.sum2[t_bdr_idx];
                }
                else if(t_lx>0 && t_uy==0 && t_fz==0)
                {
                    t1 = ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_bdl_idx];
                    t2 = ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_bdl_idx];
                }
                else if(t_lx==0 && t_uy>0 && t_fz==0)
                {
                    t1 = ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_bur_idx];
                    t2 = ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_bur_idx];
                }
                else if(t_lx==0 && t_uy==0 && t_fz>0)
                {
                    t1 = ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_fdr_idx];
                    t2 = ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_fdr_idx];
                }
                else if(t_lx>0 && t_uy>0 && t_fz==0)
                {
                    t1 = ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_bdl_idx] - ncctar.sum1[t_bur_idx] + ncctar.sum1[t_bul_idx];
                    t2 = ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_bdl_idx] - ncctar.sum2[t_bur_idx] + ncctar.sum2[t_bul_idx];
                }
                else if(t_lx>0 && t_uy==0 && t_fz>0)
                {
                    t1 = ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_bdl_idx] - ncctar.sum1[t_fdr_idx] + ncctar.sum1[t_fdl_idx];
                    t2 = ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_bdl_idx] - ncctar.sum2[t_fdr_idx] + ncctar.sum2[t_fdl_idx];
                }
                else if(t_lx==0 && t_uy>0 && t_fz>0)
                {
                    t1 = ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_bur_idx] - ncctar.sum1[t_fdr_idx] + ncctar.sum1[t_fur_idx];
                    t2 = ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_bur_idx] - ncctar.sum2[t_fdr_idx] + ncctar.sum2[t_fur_idx];
                }
                else
                {
                    t1 = fabs( ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_bdl_idx] - ncctar.sum1[t_bur_idx] - ncctar.sum1[t_fdr_idx] + ncctar.sum1[t_bul_idx] + ncctar.sum1[t_fdl_idx] + ncctar.sum1[t_fur_idx] - ncctar.sum1[t_ful_idx]);
                    t2 = fabs( ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_bdl_idx] - ncctar.sum2[t_bur_idx] - ncctar.sum2[t_fdr_idx] + ncctar.sum2[t_bul_idx] + ncctar.sum2[t_fdl_idx] + ncctar.sum2[t_fur_idx] - ncctar.sum2[t_ful_idx]);
                }
                t_mean = t1/len_t;
                t_std = sqrt(y_max(t2 - t1*t_mean, T1(0) ));
                T1 s1,s2;
                if(s_lx==0 && s_uy==0 && s_fz==0)
                {
                    s1 = nccsub.sum1[s_bdr_idx];
                    s2 = nccsub.sum2[s_bdr_idx];
                }
                else if(s_lx>0 && s_uy==0 && s_fz==0)
                {
                    s1 = nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_bdl_idx];
                    s2 = nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_bdl_idx];
                }
                else if(s_lx==0 && s_uy>0 && s_fz==0)
                {
                    s1 = nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_bur_idx];
                    s2 = nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_bur_idx];
                }
                else if(s_lx==0 && s_uy==0 && s_fz>0)
                {
                    s1 = nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_fdr_idx];
                    s2 = nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_fdr_idx];
                }
                else if(s_lx>0 && s_uy>0 && s_fz==0)
                {
                    s1 = nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_bdl_idx] - nccsub.sum1[s_bur_idx] + nccsub.sum1[s_bul_idx];
                    s2 = nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_bdl_idx] - nccsub.sum2[s_bur_idx] + nccsub.sum2[s_bul_idx];
                }
                else if(s_lx>0 && s_uy==0 && s_fz>0)
                {
                    s1 = nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_bdl_idx] - nccsub.sum1[s_fdr_idx] + nccsub.sum1[s_fdl_idx];
                    s2 = nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_bdl_idx] - nccsub.sum2[s_fdr_idx] + nccsub.sum2[s_fdl_idx];
                }
                else if(s_lx==0 && s_uy>0 && s_fz>0)
                {
                    s1 = nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_bur_idx] - nccsub.sum1[s_fdr_idx] + nccsub.sum1[s_fur_idx];
                    s2 = nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_bur_idx] - nccsub.sum2[s_fdr_idx] + nccsub.sum2[s_fur_idx];
                }
                else
                {
                    s1 = fabs( nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_bdl_idx] - nccsub.sum1[s_bur_idx] - nccsub.sum1[s_fdr_idx] + nccsub.sum1[s_bul_idx] + nccsub.sum1[s_fdl_idx] + nccsub.sum1[s_fur_idx] - nccsub.sum1[s_ful_idx]);
                    s2 = fabs( nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_bdl_idx] - nccsub.sum2[s_bur_idx] - nccsub.sum2[s_fdr_idx] + nccsub.sum2[s_bul_idx] + nccsub.sum2[s_fdl_idx] + nccsub.sum2[s_fur_idx] - nccsub.sum2[s_ful_idx]);
                }
                s_mean = s1/len_s;
                s_std = sqrt(y_max(s2 - s1*s_mean, T1(0) ));
                // response [0, 1] (instead of [-1, 1])
                if(t_std!=0 && s_std!=0)
                {
                    pOut.pImg[idx] = 0.5 + 0.5*(pOut.pImg[idx] - t1*s_mean)/(t_std*s_std);
                }
                else
                {
                    pOut.pImg[idx] = 0;
                }
            }
        }
    }
}
// fft ncc ND
template <class T1, class T2, class Y_IMG1, class Y_IMG2> void YImg<T1, T2, Y_IMG1, Y_IMG2> :: fftnccND(Y_IMG1 pOut, Y_IMG2 pIn, T2 *sznd, T1 overlap_percent)
{
}
// func pc & cc
// only support fft_in_place now
template <class T1, class T2, class Y_IMG1, class Y_IMG2> void YImg<T1, T2, Y_IMG1, Y_IMG2> :: fftpccc3D(Y_IMG1 pOut, Y_IMG2 pIn, T2 even_odd, bool fftwf_in_place, PEAKSLIST *peakList)
{
    // peaks
    PEAKS pos;
    T1 max_v=0;
    T2 sx_pad, sy_pad, sz_pad;
    sx_pad=pIn.sz[0];
    sy_pad=pIn.sz[1];
    sz_pad=pIn.sz[2];
    T2 len_pad = sx_pad*sy_pad*sz_pad;
    T1* pTmpOut = 0;
    try
    {
        pTmpOut = new T1 [len_pad];
    }
    catch (...)
    {
        printf("Fail to allocate memory.\n");
        return;
    }
    //
    //fftwf_init_threads(); //pthread
    //fftwf_plan_with_nthreads(PROCESSORS);
    fftwf_plan p;
    if(fftwf_in_place)
    {
        T2 sx_pad_ori = sx_pad - (2-even_odd); //2*(sx_pad/2-1);
        T2 len_pad_tmp = sx_pad_ori*sy_pad*sz_pad;
        p = fftwf_plan_dft_r2c_3d(sz_pad, sy_pad, sx_pad_ori, pIn.pImg, (fftwf_complex*)pIn.pImg, FFTW_ESTIMATE);
        fftwf_execute(p);
        fftwf_destroy_plan(p);
        p = fftwf_plan_dft_r2c_3d(sz_pad, sy_pad, sx_pad_ori, pOut.pImg, (fftwf_complex*)pOut.pImg, FFTW_ESTIMATE);
        fftwf_execute(p);
        fftwf_destroy_plan(p);
        for(T2 i=0; i<len_pad; i++)
        {
            pTmpOut[i] = pOut.pImg[i];
        }
        // compute pc & cc
        for(T2 k=0; k<sz_pad; k++)
        {
            T2 offset_k = k*sy_pad*sx_pad;
            for(T2 j=0; j<sy_pad; j++)
            {
                T2 offset_j = offset_k + j*sx_pad;
                for(T2 i=0; i<sx_pad; i+=2)
                {
                    T2 idx = offset_j + i;
                    T1 tmp = pOut.pImg[idx];
                    // obtain cc
                    pOut.pImg[idx+1] = -pOut.pImg[idx+1]; //conjugate
                    pOut.pImg[idx] = pIn.pImg[idx]*tmp - pIn.pImg[idx+1]*pOut.pImg[idx+1];
                    pOut.pImg[idx+1] = pIn.pImg[idx+1]*tmp + pIn.pImg[idx]*pOut.pImg[idx+1];
                    // obtain the cross power spectrum
                    pTmpOut[idx] = pOut.pImg[idx];
                    pTmpOut[idx+1] = pOut.pImg[idx+1];
                    T1 tmp2 = sqrt(pTmpOut[idx]*pTmpOut[idx] + pTmpOut[idx+1]*pTmpOut[idx+1]);
                    pTmpOut[idx] /= tmp2;
                    pTmpOut[idx+1] /= tmp2;
                }
            }
        }
        // obtain the phase correlation
        fftwf_complex* out_pc_sub = (fftwf_complex*)pTmpOut;
        p = fftwf_plan_dft_c2r_3d(sz_pad, sy_pad, sx_pad_ori, out_pc_sub, pTmpOut, FFTW_ESTIMATE);
        fftwf_execute(p);
        fftwf_destroy_plan(p);
        // obtain cross-correlation
        fftwf_complex* out_sub = (fftwf_complex*)pOut.pImg;
        p = fftwf_plan_dft_c2r_3d(sz_pad, sy_pad, sx_pad_ori, out_sub, pOut.pImg, FFTW_ESTIMATE);
        fftwf_execute(p);
        fftwf_destroy_plan(p);
        // cleanup
        // fftwf_cleanup_threads();
        // normalize
        for(T2 k=0; k<sz_pad; k++)
        {
            T2 offset_k = k*sy_pad*sx_pad;
            for(T2 j=0; j<sy_pad; j++)
            {
                T2 offset_j = offset_k + j*sx_pad;
                for(T2 i=0; i<sx_pad; i++)
                {
                    T2 idx = offset_j + i;
                    if(i<sx_pad_ori)
                    {
                        pOut.pImg[idx] /= (T1)len_pad_tmp; //
                        pTmpOut[idx] /= (T1)len_pad_tmp;
                    }
                    else
                    {
                        pOut.pImg[idx] = 0;
                        pTmpOut[idx] = 0;
                    }
                    // statistics of peaks
                    if(pTmpOut[idx] >= max_v)
                    {
                        pos.x = i; pos.y = j; pos.z = k; pos.value = pTmpOut[idx];
                        if(peakList->size()<1)
                            peakList->push_back(pos);
                        else
                        {
                            for(unsigned int it=peakList->size(); it>0; it--)
                            {
                                if(pos.value<=peakList->at(it-1).value)
                                {
                                    peakList->insert(peakList->begin() + it, 1, pos);
                                    if(peakList->size()>NPEAKS)
                                        peakList->erase(peakList->end()-1);
                                    break;
                                }
                                else
                                    continue;
                            }
                            if(pos.value>peakList->at(0).value)
                                peakList->insert(peakList->begin(), pos);
                            if(peakList->size()>NPEAKS)
                                peakList->erase(peakList->end()-1);
                        }
                        if(peakList->size()<NPEAKS)
                            max_v = 0;
                        else
                            max_v = peakList->at(NPEAKS-1).value;
                    }
                }
            }
        }
    }
    qDebug() << "peaks number ..." << peakList->size() << " max val ..." << max_v;
    qDebug() << "peaklist ..." << peakList->at(0).value << peakList->at(1).value << peakList->at(2).value << peakList->at(3).value << peakList->at(4).value << peakList->at(5).value << peakList->at(6).value << peakList->at(7).value;
    //de-alloc
    if(pTmpOut) {delete []pTmpOut; pTmpOut=0;}
}
//func fft phase-correlation combined with normalized cross-correlation using 1d fft
template <class T1, class T2, class Y_IMG1, class Y_IMG2> void YImg<T1, T2, Y_IMG1, Y_IMG2> :: fftpatientpccc3D(Y_IMG1 pOut, Y_IMG2 pIn, T2 even_odd, bool fftwf_in_place, PEAKSLIST *peakList)
{
    // peaks
    PEAKS pos;
    T1 max_v=0;
    T2 sx_pad, sy_pad, sz_pad;
    sx_pad=pIn.sz[0];
    sy_pad=pIn.sz[1];
    sz_pad=pIn.sz[2];
    T2 len_pad = sx_pad*sy_pad*sz_pad;
    T1* pTmpOut = 0;
    try
    {
        pTmpOut = new T1 [len_pad];
    }
    catch (...)
    {
        printf("Fail to allocate memory.\n");
        return;
    }
    //
    //fftwf_init_threads(); //pthread
    //fftwf_plan_with_nthreads(PROCESSORS);
    fftwf_plan p;
    T1 *pIm;
    fftwf_complex* pComplex;
    if(fftwf_in_place)
    {
        T2 sx_pad_ori = sx_pad - (2-even_odd); //2*(sx_pad/2-1);
        T2 len_pad_tmp = sx_pad_ori*sy_pad*sz_pad;
        pIm = pIn.pImg;
        pComplex = (fftwf_complex*)pIn.pImg;
        p = fftwf_plan_dft_r2c_3d(sz_pad, sy_pad, sx_pad_ori, pIm, pComplex, FFTW_ESTIMATE);
        fftwf_execute(p);
        pIm = pOut.pImg;
        pComplex = (fftwf_complex*)pOut.pImg;
        fftwf_execute(p);
        // de-alloc
        fftwf_destroy_plan(p);
        for(T2 i=0; i<len_pad; i++)
        {
            pTmpOut[i] = pOut.pImg[i];
        }
        // compute pc & cc
        for(T2 k=0; k<sz_pad; k++)
        {
            T2 offset_k = k*sy_pad*sx_pad;
            for(T2 j=0; j<sy_pad; j++)
            {
                T2 offset_j = offset_k + j*sx_pad;
                for(T2 i=0; i<sx_pad; i+=2)
                {
                    T2 idx = offset_j + i;
                    T1 tmp = pOut.pImg[idx];
                    // obtain cc
                    pOut.pImg[idx+1] = -pOut.pImg[idx+1]; //conjugate
                    pOut.pImg[idx] = pIn.pImg[idx]*tmp - pIn.pImg[idx+1]*pOut.pImg[idx+1];
                    pOut.pImg[idx+1] = pIn.pImg[idx+1]*tmp + pIn.pImg[idx]*pOut.pImg[idx+1];
                    // obtain the cross power spectrum
                    pTmpOut[idx] = pOut.pImg[idx];
                    pTmpOut[idx+1] = pOut.pImg[idx+1];
                    T1 tmp2 = sqrt(power(pTmpOut[idx], 2) + power(pTmpOut[idx+1], 2));
                    pTmpOut[idx] /= tmp2;
                    pTmpOut[idx+1] /= tmp2;
                }
            }
        }
        // obtain the phase correlation
        pIm = pTmpOut;
        pComplex = (fftwf_complex*)pTmpOut;
        p = fftwf_plan_dft_c2r_3d(sz_pad, sy_pad, sx_pad_ori, pComplex, pIm, FFTW_ESTIMATE);
        fftwf_execute(p);
        // obtain cross-correlation
        pIm = pOut.pImg;
        pComplex = (fftwf_complex*)pOut.pImg;
        fftwf_execute(p);
        // de-alloc
        fftwf_destroy_plan(p);
        // normalize
        for(T2 k=0; k<sz_pad; k++)
        {
            T2 offset_k = k*sy_pad*sx_pad;
            for(T2 j=0; j<sy_pad; j++)
            {
                T2 offset_j = offset_k + j*sx_pad;
                for(T2 i=0; i<sx_pad; i++)
                {
                    T2 idx = offset_j + i;
                    if(i<sx_pad_ori)
                    {
                        pOut.pImg[idx] /= (T1)len_pad_tmp; //
                        pTmpOut[idx] /= (T1)len_pad_tmp;
                    }
                    else
                    {
                        pOut.pImg[idx] = 0;
                        pTmpOut[idx] = 0;
                    }
                    // statistics of peaks
                    if(pTmpOut[idx] > max_v)
                    {
                        max_v = pTmpOut[idx];
                        pos.x = i; pos.y = j; pos.z = k; pos.value = max_v;
                        if(peakList->size()<1)
                            peakList->push_back(pos);
                        else
                        {
                            for(unsigned int it=peakList->size(); it!=0; it--)
                            {
                                if(pos.value>=peakList->at(it-1).value)
                                {
                                    peakList->insert(peakList->begin() + it, 1, pos);
                                    if(peakList->size()>NPEAKS)
                                        peakList->erase(peakList->begin());
                                    break;
                                }
                                else
                                    continue;
                            }
                            if(pos.value<peakList->at(0).value)
                                peakList->insert(peakList->begin(), pos);
                            if(peakList->size()>NPEAKS)
                                peakList->erase(peakList->begin());
                        }
                    }
                }
            }
        }
    }
    //de-alloc
    if(pTmpOut) {delete []pTmpOut; pTmpOut=0;}
    // cleanup
    //fftwf_cleanup_threads();
}
//func fft phase-correlation in subpixel extension
template <class T1, class T2, class Y_IMG1, class Y_IMG2> void YImg<T1, T2, Y_IMG1, Y_IMG2> :: fftpcsubspace3D(Y_IMG1 pOut, Y_IMG2 pIn, T2 even_odd, bool fftwf_in_place, rPEAKS *pos)
{
    //
    T1 max_v = -INF;
    T2 sx_pad, sy_pad, sz_pad;
    sx_pad=pIn.sz[0];
    sy_pad=pIn.sz[1];
    sz_pad=pIn.sz[2];
    T2 dimx = (sx_pad+1 - (2-even_odd))/2;
    T2 dimy = (sy_pad+1)/2;
    T2 dimz = (sz_pad+1)/2;
    T2 len_pad = sx_pad*sy_pad*sz_pad;
    //
    //fftwf_init_threads(); //pthread
    //fftwf_plan_with_nthreads(PROCESSORS);
    fftwf_plan p;
    T1 x=0,y=0,z=0;
    if(fftwf_in_place)
    {
        T2 sx_pad_ori = sx_pad - (2-even_odd); //2*(sx_pad/2-1);
        T2 len_pad_tmp = sx_pad_ori*sy_pad*sz_pad;
        p = fftwf_plan_dft_r2c_3d(sz_pad, sy_pad, sx_pad_ori, pIn.pImg, (fftwf_complex*)pIn.pImg, FFTW_ESTIMATE);
        fftwf_execute(p);
        fftwf_destroy_plan(p);
        p = fftwf_plan_dft_r2c_3d(sz_pad, sy_pad, sx_pad_ori, pOut.pImg, (fftwf_complex*)pOut.pImg, FFTW_ESTIMATE);
        fftwf_execute(p);
        fftwf_destroy_plan(p);
        // compute pc
        for(T2 k=0; k<sz_pad; k++)
        {
            T2 offset_k = k*sy_pad*sx_pad;
            for(T2 j=0; j<sy_pad; j++)
            {
                T2 offset_j = offset_k + j*sx_pad;
                for(T2 i=0; i<sx_pad; i+=2)
                {
                    T2 idx = offset_j + i;
                    T1 tmp = pOut.pImg[idx];
                    // obtain cross correlation
                    pOut.pImg[idx+1] = -pOut.pImg[idx+1]; //conjugate
                    pOut.pImg[idx] = pIn.pImg[idx]*tmp - pIn.pImg[idx+1]*pOut.pImg[idx+1];
                    pOut.pImg[idx+1] = pIn.pImg[idx+1]*tmp + pIn.pImg[idx]*pOut.pImg[idx+1];
                    // obtain normalized cross power spectrum
                    T1 tmp2 = sqrt(pOut.pImg[idx]*pOut.pImg[idx] + pOut.pImg[idx+1]*pOut.pImg[idx+1]) + EPS;
                    pOut.pImg[idx] /= tmp2;
                    pOut.pImg[idx+1] /= tmp2;
                    
                }
            }
        }
        // obtain the phase correlation
        fftwf_complex* out_pc_sub = (fftwf_complex*)pOut.pImg;
        p = fftwf_plan_dft_c2r_3d(sz_pad, sy_pad, sx_pad_ori, out_pc_sub, pOut.pImg, FFTW_ESTIMATE);
        fftwf_execute(p);
        fftwf_destroy_plan(p);
        // cleanup
        // fftwf_cleanup_threads();
        // normalizing
        for(T2 k=0; k<sz_pad; k++)
        {
            T2 offset_k = k*sy_pad*sx_pad;
            for(T2 j=0; j<sy_pad; j++)
            {
                T2 offset_j = offset_k + j*sx_pad;
                for(T2 i=0; i<sx_pad; i++)
                {
                    T2 idx = offset_j + i;
                    if(i<sx_pad_ori)
                    {
                        pOut.pImg[idx] /= (T1)len_pad_tmp; //
                    }
                    else
                    {
                        pOut.pImg[idx] = 0;
                    }
                    // peak
                    if(pOut.pImg[idx] > max_v)
                    {
                        max_v = pOut.pImg[idx];
                        x = i; y = j; z = k; pos->value = max_v;
                    }
                }
            }
        }
    }
    else
    {
        // need more memory
        // not support
    }
    qDebug()<<"integer shifts ... "<<x<<y<<z<<" dims ..."<<sx_pad<<sy_pad<<sz_pad<<"max_v ..."<<max_v;
    T2 idxcur = z*sy_pad*sx_pad + y*sx_pad + x;
    T2 xp = 1;
    T2 yp = sx_pad;
    T2 zp = sy_pad*sx_pad;
    qDebug()<<"test ..."<<idxcur<<idxcur+xp<<pOut.pImg[idxcur]<<" x "<<pOut.pImg[idxcur+xp]<<" y "<<idxcur+yp<<pOut.pImg[idxcur+yp]<<" z "<<idxcur+zp<<pOut.pImg[idxcur+zp];
    
    x -= dimx-1; //
    y -= dimy-1; //
    z -= dimz-1; //
    qDebug()<<"x, y, z ..."<<x<<y<<z;
    T1 sign_x = 1.0;
    T1 sign_y = 1.0;
    T1 sign_z = 1.0;
    if(sx_pad%2==0) sign_x = -1.0;
    if(sy_pad%2==0) sign_y = -1.0;
    if(sz_pad%2==0) sign_z = -1.0;
    qDebug()<<"test ..."<<sign_x<<sign_y<<sign_z;
    pos->x = x + (pOut.pImg[idxcur+xp])/(pOut.pImg[idxcur+xp] + sign_x * pOut.pImg[idxcur]);
    if(y_abs<T1>(pos->x-x)>1) // result should be in the interval [-1, 1]
    {
        sign_x *= -1.0;
        pos->x = x + (pOut.pImg[idxcur+xp])/(pOut.pImg[idxcur+xp] + sign_x * pOut.pImg[idxcur]);
    }
    pos->y = y + (pOut.pImg[idxcur+yp])/(pOut.pImg[idxcur+yp] + sign_y * pOut.pImg[idxcur]);
    if(y_abs<T1>(pos->y-y)>1)
    {
        sign_y *= -1.0;
        pos->y = y + (pOut.pImg[idxcur+yp])/(pOut.pImg[idxcur+yp] + sign_y * pOut.pImg[idxcur]);
    }
    pos->z = z + (pOut.pImg[idxcur+zp])/(pOut.pImg[idxcur+zp] + sign_z * pOut.pImg[idxcur]);
    if(y_abs<T1>(pos->z-z)>1)
    {
        sign_z *= -1.0;
        pos->z = z + (pOut.pImg[idxcur+zp])/(pOut.pImg[idxcur+zp] + sign_z * pOut.pImg[idxcur]);
    }
    
    qDebug()<<"subspace shifts ... "<<pos->x<<pos->y<<pos->z;
    //
    return;
}
// fft pc-ncc 3D
template <class T1, class T2, class Y_IMG1, class Y_IMG2> void YImg<T1, T2, Y_IMG1, Y_IMG2> :: fftpcncc3D(Y_IMG1 pOut, Y_IMG2 pIn, T2 *sz3d, T2 even_odd, bool fftwf_in_place, T1 overlap_percent, PEAKS *pos)
{
    T2 tx=sz3d[0], ty=sz3d[1], tz=sz3d[2];
    T2 sx_pad, sy_pad, sz_pad;
    sx_pad=pIn.sz[0];
    sy_pad=pIn.sz[1];
    sz_pad=pIn.sz[2];
    T2 len_pad = sx_pad*sy_pad*sz_pad;
    T2 sx, sy, sz;
    T2 sx_pad_ori;
    if(fftwf_in_place)
        sx_pad_ori = sx_pad - (2-even_odd); //2*(sx_pad/2-1);
    else
        sx_pad_ori = sx_pad;
    sx=sx_pad_ori+1-tx; // fftwf_in_place
    sy=sy_pad+1-ty;
    sz=sz_pad+1-tz;
    T2 sub_sz = sx*sy*sz;
    int start_t = clock();
    NST<T1, T2> ncctar(pIn.sz, pIn.pImg, even_odd, fftwf_in_place, 3);
    NST<T1, T2> nccsub(pOut.sz, pOut.pImg, even_odd, fftwf_in_place, 3);
    int end_t = clock();
    qDebug() << "time consumed ... " << end_t - start_t;
    //	//phase-correlation
    //	T1* pTmpOut = 0, *pTmpIn = 0;
    //	try
    //	{
    //		pTmpOut = new T1 [len_pad];
    //		pTmpIn = new T1 [len_pad];
    //	}
    //	catch (...)
    //	{
    //		printf("Fail to allocate memory.\n");
    //		return;
    //	}
    //
    //	for(T2 i=0; i<len_pad; i++)
    //	{
    //		pTmpOut[i] = pOut.pImg[i];
    //		pTmpIn[i] = pIn.pImg[i];
    //	}
    //
    //	Y_IMG_REAL pYIMOut(pTmpOut, pOut.sz);
    //	Y_IMG_REAL pYIMIn(pTmpIn, pIn.sz);
    //
    //	PEAKSLIST peakList;
    //	fftpc3D(pYIMOut, pYIMIn, even_odd, fftwf_in_place, &peakList);
    //
    //	if(pTmpOut) {delete []pTmpOut; pTmpOut=0;}
    //	if(pTmpIn) {delete []pTmpIn; pTmpIn=0;}
    //
    //
    //	//cross-correlation
    //	fftcc3D(pOut, pIn, even_odd, fftwf_in_place);
    // pc and cc
    PEAKSLIST peakList;
    fftpccc3D(pOut, pIn, even_odd, fftwf_in_place, &peakList);
    //fftpatientpccc3D(pOut, pIn, even_odd, fftwf_in_place, &peakList);
    int end_tt = clock();
    qDebug() << "pccc time consumed ... " << end_tt - end_t;
    qDebug() << "how many peaks found ..." << peakList.size();
    //	// for test
    //	qDebug() << "pOut ..." << pOut.pImg[33486237] << 33486237;
    qDebug() <<"sz ..."<<sx_pad*sy_pad*sz_pad<<sx_pad_ori*sy_pad*sz_pad << sx_pad_ori<<sy_pad<<sz_pad<<sx<<sy<<sz<<tx<<ty<<tz;
    //ncc
    pos->value = 0;
    //
    if(peakList.size() > 0)
    {
        for(T2 ii=peakList.size()-1; ii!=0; ii--)
        {
            T2 pos_x = peakList.at(ii).x;
            T2 pos_y = peakList.at(ii).y;
            T2 pos_z = peakList.at(ii).z;
            qDebug() << "peaks ..." << pos_x << pos_y << pos_z;
            // bounding box
            T2 nbb = 3;
            T2 w_start = pos_z - nbb; if(w_start<0) w_start = 0;
            T2 w_end = pos_z + nbb; if(w_end>sz_pad) w_end = sz_pad;
            T2 v_start = pos_y - nbb; if(v_start<0) v_start = 0;
            T2 v_end = pos_y + nbb; if(v_end>sy_pad) v_end = sy_pad;
            T2 u_start = pos_x - nbb; if(u_start<0) u_start = 0;
            T2 u_end = pos_x + nbb; if(u_end>sx_pad) u_end = sx_pad;
            // computing ncc
            T2 offset_z = sx_pad_ori*sy_pad;
            for(T2 w=w_start; w<w_end; w++)
            {
                T2 offset_w = w*sx_pad*sy_pad;
                for(T2 v=v_start; v<v_end; v++)
                {
                    T2 offset_v = offset_w + v*sx_pad;
                    for(T2 u=u_start; u<u_end; u++) // fftwf_in_place
                    {
                        T2 idx = offset_v + u;
                        // z - (front, back) y - (upper, down) x - (left, right)
                        T2 t_ful_idx, t_fur_idx, t_fdl_idx, t_fdr_idx, t_bul_idx, t_bur_idx, t_bdl_idx, t_bdr_idx;
                        T2 s_ful_idx, s_fur_idx, s_fdl_idx, s_fdr_idx, s_bul_idx, s_bur_idx, s_bdl_idx, s_bdr_idx;
                        T2 t_lx, t_rx, t_uy, t_dy, t_fz, t_bz;
                        T2 s_lx, s_rx, s_uy, s_dy, s_fz, s_bz;
                        T1 t_std, t_mean;
                        T1 s_std, s_mean;
                        if(u<sx && u<tx)
                        {
                            s_lx = sx-1 - u; s_rx = sx-1;
                            t_lx = sx-1; t_rx = sx-1 + u;
                        }
                        else if(u<sx && u>=tx)
                        {
                            s_lx = sx-1 - u; s_rx = sx_pad_ori-1 - u;
                            t_lx = sx-1; t_rx = sx_pad_ori-1;
                        }
                        else if(u>=sx && u<tx)
                        {
                            s_lx = 0; s_rx = sx-1;
                            t_lx = u; t_rx = sx-1 + u;
                        }
                        else if(u>=tx)
                        {
                            s_lx = 0; s_rx = sx_pad_ori-1 - u;
                            t_lx = u; t_rx = sx_pad_ori-1;
                        }
                        else
                            printf("x direction %ld %ld %ld %ld \n", u, sx, tx, sx_pad);
                        if(v<sy && v<ty)
                        {
                            s_uy = sy-1 - v; s_dy = sy-1;
                            t_uy = sy-1; t_dy = sy-1 + v;
                        }
                        else if(v<sy && v>=ty)
                        {
                            s_uy = sy-1 - v; s_dy = sy_pad-1 - v;
                            t_uy = sy-1; t_dy = sy_pad-1;
                        }
                        else if(v<ty && v>=sy)
                        {
                            s_uy = 0; s_dy = sy-1;
                            t_uy = v; t_dy = sy-1 + v;
                        }
                        else if(v>=ty)
                        {
                            s_uy = 0; s_dy = sy_pad-1 - v;
                            t_uy = v; t_dy = sy_pad-1;
                        }
                        else
                            printf("y direction %ld %ld %ld %ld \n", v, sy, ty, sy_pad);
                        if(w<sz && w<tz)
                        {
                            s_fz = sz-1 - w; s_bz = sz-1;
                            t_fz = sz-1; t_bz = sz-1 + w;
                        }
                        else if(w<sz && w>=tz)
                        {
                            s_fz = sz-1 - w; s_bz = sz_pad-1 - w;
                            t_fz = sz-1; t_bz = sz_pad-1;
                        }
                        else if(w<tz && w>=sz)
                        {
                            s_fz = 0; s_bz = sz-1;
                            t_fz = w; t_bz = sz-1 + w;
                        }
                        else if(w>=tz)
                        {
                            s_fz = 0; s_bz = sz_pad-1 - w;
                            t_fz = w; t_bz = sz_pad-1;
                        }
                        else
                            printf("z direction %ld %ld %ld %ld \n", w, sz, tz, sz_pad);
                        if(t_rx<t_lx || t_dy<t_uy || t_bz<t_fz || s_rx<s_lx || s_dy<s_uy || s_bz<s_fz)
                        {
                            pOut.pImg[idx] = 0;
                            continue;
                        }
                        // overlap boundary
                        t_ful_idx = (t_fz-1)*offset_z + (t_uy-1)*sx_pad_ori + t_lx-1;
                        t_fur_idx = (t_fz-1)*offset_z + (t_uy-1)*sx_pad_ori + t_rx;
                        t_fdl_idx = (t_fz-1)*offset_z + t_dy*sx_pad_ori + t_lx-1;
                        t_fdr_idx = (t_fz-1)*offset_z + t_dy*sx_pad_ori + t_rx;
                        t_bul_idx = t_bz*offset_z + (t_uy-1)*sx_pad_ori + t_lx-1;
                        t_bur_idx = t_bz*offset_z + (t_uy-1)*sx_pad_ori + t_rx;
                        t_bdl_idx = t_bz*offset_z + t_dy*sx_pad_ori + t_lx-1;
                        t_bdr_idx = t_bz*offset_z + t_dy*sx_pad_ori + t_rx;
                        s_ful_idx = (s_fz-1)*offset_z + (s_uy-1)*sx_pad_ori + s_lx-1;
                        s_fur_idx = (s_fz-1)*offset_z + (s_uy-1)*sx_pad_ori + s_rx;
                        s_fdl_idx = (s_fz-1)*offset_z + s_dy*sx_pad_ori + s_lx-1;
                        s_fdr_idx = (s_fz-1)*offset_z + s_dy*sx_pad_ori + s_rx;
                        s_bul_idx = s_bz*offset_z + (s_uy-1)*sx_pad_ori + s_lx-1;
                        s_bur_idx = s_bz*offset_z + (s_uy-1)*sx_pad_ori + s_rx;
                        s_bdl_idx = s_bz*offset_z + s_dy*sx_pad_ori + s_lx-1;
                        s_bdr_idx = s_bz*offset_z + s_dy*sx_pad_ori + s_rx;
                        T2 len_t = (t_bz - t_fz + 1)*(t_dy - t_uy + 1)*(t_rx - t_lx + 1), len_s = (s_bz - s_fz + 1)*(s_dy - s_uy + 1)*(s_rx - s_lx + 1);
                        // prior knowledge of ratio of overlap
                        if(T1(len_s)<T1(sub_sz)*overlap_percent)
                        {
                            pOut.pImg[idx] = 0;
                            continue;
                        }
                        T1 t1,t2;
                        if(t_lx==0 && t_uy==0 && t_fz==0)
                        {
                            t1 = ncctar.sum1[t_bdr_idx];
                            t2 = ncctar.sum2[t_bdr_idx];
                        }
                        else if(t_lx>0 && t_uy==0 && t_fz==0)
                        {
                            t1 = ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_bdl_idx];
                            t2 = ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_bdl_idx];
                        }
                        else if(t_lx==0 && t_uy>0 && t_fz==0)
                        {
                            t1 = ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_bur_idx];
                            t2 = ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_bur_idx];
                        }
                        else if(t_lx==0 && t_uy==0 && t_fz>0)
                        {
                            t1 = ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_fdr_idx];
                            t2 = ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_fdr_idx];
                        }
                        else if(t_lx>0 && t_uy>0 && t_fz==0)
                        {
                            t1 = ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_bdl_idx] - ncctar.sum1[t_bur_idx] + ncctar.sum1[t_bul_idx];
                            t2 = ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_bdl_idx] - ncctar.sum2[t_bur_idx] + ncctar.sum2[t_bul_idx];
                        }
                        else if(t_lx>0 && t_uy==0 && t_fz>0)
                        {
                            t1 = ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_bdl_idx] - ncctar.sum1[t_fdr_idx] + ncctar.sum1[t_fdl_idx];
                            t2 = ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_bdl_idx] - ncctar.sum2[t_fdr_idx] + ncctar.sum2[t_fdl_idx];
                        }
                        else if(t_lx==0 && t_uy>0 && t_fz>0)
                        {
                            t1 = ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_bur_idx] - ncctar.sum1[t_fdr_idx] + ncctar.sum1[t_fur_idx];
                            t2 = ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_bur_idx] - ncctar.sum2[t_fdr_idx] + ncctar.sum2[t_fur_idx];
                        }
                        else
                        {
                            t1 = fabs( ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_bdl_idx] - ncctar.sum1[t_bur_idx] - ncctar.sum1[t_fdr_idx] + ncctar.sum1[t_bul_idx] + ncctar.sum1[t_fdl_idx] + ncctar.sum1[t_fur_idx] - ncctar.sum1[t_ful_idx]);
                            t2 = fabs( ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_bdl_idx] - ncctar.sum2[t_bur_idx] - ncctar.sum2[t_fdr_idx] + ncctar.sum2[t_bul_idx] + ncctar.sum2[t_fdl_idx] + ncctar.sum2[t_fur_idx] - ncctar.sum2[t_ful_idx]);
                        }
                        t_mean = t1/len_t;
                        t_std = sqrt(y_max(t2 - t1*t_mean, T1(0) ));
                        T1 s1,s2;
                        if(s_lx==0 && s_uy==0 && s_fz==0)
                        {
                            s1 = nccsub.sum1[s_bdr_idx];
                            s2 = nccsub.sum2[s_bdr_idx];
                        }
                        else if(s_lx>0 && s_uy==0 && s_fz==0)
                        {
                            s1 = nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_bdl_idx];
                            s2 = nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_bdl_idx];
                        }
                        else if(s_lx==0 && s_uy>0 && s_fz==0)
                        {
                            s1 = nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_bur_idx];
                            s2 = nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_bur_idx];
                        }
                        else if(s_lx==0 && s_uy==0 && s_fz>0)
                        {
                            s1 = nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_fdr_idx];
                            s2 = nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_fdr_idx];
                        }
                        else if(s_lx>0 && s_uy>0 && s_fz==0)
                        {
                            s1 = nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_bdl_idx] - nccsub.sum1[s_bur_idx] + nccsub.sum1[s_bul_idx];
                            s2 = nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_bdl_idx] - nccsub.sum2[s_bur_idx] + nccsub.sum2[s_bul_idx];
                        }
                        else if(s_lx>0 && s_uy==0 && s_fz>0)
                        {
                            s1 = nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_bdl_idx] - nccsub.sum1[s_fdr_idx] + nccsub.sum1[s_fdl_idx];
                            s2 = nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_bdl_idx] - nccsub.sum2[s_fdr_idx] + nccsub.sum2[s_fdl_idx];
                        }
                        else if(s_lx==0 && s_uy>0 && s_fz>0)
                        {
                            s1 = nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_bur_idx] - nccsub.sum1[s_fdr_idx] + nccsub.sum1[s_fur_idx];
                            s2 = nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_bur_idx] - nccsub.sum2[s_fdr_idx] + nccsub.sum2[s_fur_idx];
                        }
                        else
                        {
                            s1 = fabs( nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_bdl_idx] - nccsub.sum1[s_bur_idx] - nccsub.sum1[s_fdr_idx] + nccsub.sum1[s_bul_idx] + nccsub.sum1[s_fdl_idx] + nccsub.sum1[s_fur_idx] - nccsub.sum1[s_ful_idx]);
                            s2 = fabs( nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_bdl_idx] - nccsub.sum2[s_bur_idx] - nccsub.sum2[s_fdr_idx] + nccsub.sum2[s_bul_idx] + nccsub.sum2[s_fdl_idx] + nccsub.sum2[s_fur_idx] - nccsub.sum2[s_ful_idx]);
                        }
                        s_mean = s1/len_s;
                        s_std = sqrt(y_max(s2 - s1*s_mean, T1(0) ));
                        // response [0, 1] (instead of [-1, 1])
                        if(t_std!=0 && s_std!=0)
                        {
                            T1 tmp = pOut.pImg[idx];
                            tmp = 0.5 + 0.5*(tmp - t1*s_mean)/(t_std*s_std);
                            if(tmp>pos->value)
                            {
                                pos->value = tmp;
                                pos->x = u; pos->y = v; pos->z = w;
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        cout << "Fail of finding phase-correlation peaks" << endl;
        pos->value = -1;
        return;
    }
}
// fft ncc 3d in finer scale with one peak from coarser scale 
template <class T1, class T2, class Y_IMG1, class Y_IMG2> void YImg<T1, T2, Y_IMG1, Y_IMG2> :: fftnccp3D(Y_IMG1 pOut, Y_IMG2 pIn, T2 *sz_sub, T2 *sz_tar, T2 even_odd, bool fftwf_in_place, T1 *scale, PEAKS *pos)
{
    T2 tx=sz_tar[0], ty=sz_tar[1], tz=sz_tar[2];
    T2 sx_pad, sy_pad, sz_pad;
    sx_pad=pIn.sz[0];
    sy_pad=pIn.sz[1];
    sz_pad=pIn.sz[2];
    T2 len_pad = sx_pad*sy_pad*sz_pad;
    T2 sx, sy, sz;
    T2 sx_pad_ori;
    if(fftwf_in_place)
        sx_pad_ori = sx_pad - (2-even_odd); //2*(sx_pad/2-1);
    else
        sx_pad_ori = sx_pad;
    sx=sx_pad_ori+1-tx; // fftwf_in_place
    sy=sy_pad+1-ty;
    sz=sz_pad+1-tz;
    T2 sub_sz = sx*sy*sz;
    NST<T1, T2> ncctar(pIn.sz, pIn.pImg, even_odd, fftwf_in_place, 3);
    NST<T1, T2> nccsub(pOut.sz, pOut.pImg, even_odd, fftwf_in_place, 3);
    //cross-correlation
    fftcc3D(pOut, pIn, even_odd, fftwf_in_place);
    T2 nbbx, nbby, nbbz;
    nbbx = 1/scale[0] + 2;
    nbby = 1/scale[1] + 2;
    nbbz = 1/scale[2] + 2;
    //ncc
    pos->value = 0;
    //
    T2 pos_x = pos->x;
    T2 pos_y = pos->y;
    T2 pos_z = pos->z;
    // bounding box
    T2 w_start = pos_z - nbbz; if(w_start<0) w_start = 0;
    T2 w_end = pos_z + nbbz; if(w_end>sz_pad) w_end = sz_pad;
    T2 v_start = pos_y - nbby; if(v_start<0) v_start = 0;
    T2 v_end = pos_y + nbby; if(v_end>sy_pad) v_end = sy_pad;
    T2 u_start = pos_x - nbbx; if(u_start<0) u_start = 0;
    T2 u_end = pos_x + nbbx; if(u_end>sx_pad_ori) u_end = sx_pad_ori;
    //qDebug()<< "bb... "<< nbbx << nbby << nbbz << w_start << w_end << v_start << v_end << u_start << u_end;
    // computing ncc
    T2 offset_z = sx_pad_ori*sy_pad;
    T2 offset_y = sx_pad_ori;
    for(T2 w=w_start; w<w_end; w++)
    {
        T2 offset_w = w*sx_pad*sy_pad;
        for(T2 v=v_start; v<v_end; v++)
        {
            T2 offset_v = offset_w + v*sx_pad;
            for(T2 u=u_start; u<u_end; u++) // fftwf_in_place
            {
                T2 idx = offset_v + u;
                // z - (front, back) y - (upper, down) x - (left, right)
                T2 t_ful_idx, t_fur_idx, t_fdl_idx, t_fdr_idx, t_bul_idx, t_bur_idx, t_bdl_idx, t_bdr_idx;
                T2 s_ful_idx, s_fur_idx, s_fdl_idx, s_fdr_idx, s_bul_idx, s_bur_idx, s_bdl_idx, s_bdr_idx;
                T2 t_lx, t_rx, t_uy, t_dy, t_fz, t_bz;
                T2 s_lx, s_rx, s_uy, s_dy, s_fz, s_bz;
                T1 t_std, t_mean;
                T1 s_std, s_mean;
                if(u<sx && u<tx)
                {
                    s_lx = sx-1 - u; s_rx = sx-1;
                    t_lx = sx-1; t_rx = sx-1 + u;
                }
                else if(u<sx && u>=tx)
                {
                    s_lx = sx-1 - u; s_rx = sx_pad_ori-1 - u;
                    t_lx = sx-1; t_rx = sx_pad_ori-1;
                }
                else if(u>=sx && u<tx)
                {
                    s_lx = 0; s_rx = sx-1;
                    t_lx = u; t_rx = sx-1 + u;
                }
                else if(u>=tx)
                {
                    s_lx = 0; s_rx = sx_pad_ori-1 - u;
                    t_lx = u; t_rx = sx_pad_ori-1;
                }
                else
                    printf("x direction %ld %ld %ld %ld \n", u, sx, tx, sx_pad);
                if(v<sy && v<ty)
                {
                    s_uy = sy-1 - v; s_dy = sy-1;
                    t_uy = sy-1; t_dy = sy-1 + v;
                }
                else if(v<sy && v>=ty)
                {
                    s_uy = sy-1 - v; s_dy = sy_pad-1 - v;
                    t_uy = sy-1; t_dy = sy_pad-1;
                }
                else if(v<ty && v>=sy)
                {
                    s_uy = 0; s_dy = sy-1;
                    t_uy = v; t_dy = sy-1 + v;
                }
                else if(v>=ty)
                {
                    s_uy = 0; s_dy = sy_pad-1 - v;
                    t_uy = v; t_dy = sy_pad-1;
                }
                else
                    printf("y direction %ld %ld %ld %ld \n", v, sy, ty, sy_pad);
                if(w<sz && w<tz)
                {
                    s_fz = sz-1 - w; s_bz = sz-1;
                    t_fz = sz-1; t_bz = sz-1 + w;
                }
                else if(w<sz && w>=tz)
                {
                    s_fz = sz-1 - w; s_bz = sz_pad-1 - w;
                    t_fz = sz-1; t_bz = sz_pad-1;
                }
                else if(w<tz && w>=sz)
                {
                    s_fz = 0; s_bz = sz-1;
                    t_fz = w; t_bz = sz-1 + w;
                }
                else if(w>=tz)
                {
                    s_fz = 0; s_bz = sz_pad-1 - w;
                    t_fz = w; t_bz = sz_pad-1;
                }
                else
                    printf("z direction %ld %ld %ld %ld \n", w, sz, tz, sz_pad);
                if(t_rx<t_lx || t_dy<t_uy || t_bz<t_fz || s_rx<s_lx || s_dy<s_uy || s_bz<s_fz)
                {
                    pOut.pImg[idx] = 0;
                    continue;
                }
                // overlap boundary
                t_ful_idx = (t_fz-1)*offset_z + (t_uy-1)*sx_pad_ori + t_lx-1;
                t_fur_idx = (t_fz-1)*offset_z + (t_uy-1)*sx_pad_ori + t_rx;
                t_fdl_idx = (t_fz-1)*offset_z + t_dy*sx_pad_ori + t_lx-1;
                t_fdr_idx = (t_fz-1)*offset_z + t_dy*sx_pad_ori + t_rx;
                t_bul_idx = t_bz*offset_z + (t_uy-1)*sx_pad_ori + t_lx-1;
                t_bur_idx = t_bz*offset_z + (t_uy-1)*sx_pad_ori + t_rx;
                t_bdl_idx = t_bz*offset_z + t_dy*sx_pad_ori + t_lx-1;
                t_bdr_idx = t_bz*offset_z + t_dy*sx_pad_ori + t_rx;
                s_ful_idx = (s_fz-1)*offset_z + (s_uy-1)*sx_pad_ori + s_lx-1;
                s_fur_idx = (s_fz-1)*offset_z + (s_uy-1)*sx_pad_ori + s_rx;
                s_fdl_idx = (s_fz-1)*offset_z + s_dy*sx_pad_ori + s_lx-1;
                s_fdr_idx = (s_fz-1)*offset_z + s_dy*sx_pad_ori + s_rx;
                s_bul_idx = s_bz*offset_z + (s_uy-1)*sx_pad_ori + s_lx-1;
                s_bur_idx = s_bz*offset_z + (s_uy-1)*sx_pad_ori + s_rx;
                s_bdl_idx = s_bz*offset_z + s_dy*sx_pad_ori + s_lx-1;
                s_bdr_idx = s_bz*offset_z + s_dy*sx_pad_ori + s_rx;
                T2 len_t = (t_bz - t_fz + 1)*(t_dy - t_uy + 1)*(t_rx - t_lx + 1), len_s = (s_bz - s_fz + 1)*(s_dy - s_uy + 1)*(s_rx - s_lx + 1);
                //
                T1 t1,t2;
                if(t_lx==0 && t_uy==0 && t_fz==0)
                {
                    t1 = ncctar.sum1[t_bdr_idx];
                    t2 = ncctar.sum2[t_bdr_idx];
                }
                else if(t_lx>0 && t_uy==0 && t_fz==0)
                {
                    t1 = ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_bdl_idx];
                    t2 = ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_bdl_idx];
                }
                else if(t_lx==0 && t_uy>0 && t_fz==0)
                {
                    t1 = ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_bur_idx];
                    t2 = ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_bur_idx];
                }
                else if(t_lx==0 && t_uy==0 && t_fz>0)
                {
                    t1 = ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_fdr_idx];
                    t2 = ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_fdr_idx];
                }
                else if(t_lx>0 && t_uy>0 && t_fz==0)
                {
                    t1 = ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_bdl_idx] - ncctar.sum1[t_bur_idx] + ncctar.sum1[t_bul_idx];
                    t2 = ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_bdl_idx] - ncctar.sum2[t_bur_idx] + ncctar.sum2[t_bul_idx];
                }
                else if(t_lx>0 && t_uy==0 && t_fz>0)
                {
                    t1 = ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_bdl_idx] - ncctar.sum1[t_fdr_idx] + ncctar.sum1[t_fdl_idx];
                    t2 = ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_bdl_idx] - ncctar.sum2[t_fdr_idx] + ncctar.sum2[t_fdl_idx];
                }
                else if(t_lx==0 && t_uy>0 && t_fz>0)
                {
                    t1 = ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_bur_idx] - ncctar.sum1[t_fdr_idx] + ncctar.sum1[t_fur_idx];
                    t2 = ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_bur_idx] - ncctar.sum2[t_fdr_idx] + ncctar.sum2[t_fur_idx];
                }
                else
                {
                    t1 = fabs( ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_bdl_idx] - ncctar.sum1[t_bur_idx] - ncctar.sum1[t_fdr_idx] + ncctar.sum1[t_bul_idx] + ncctar.sum1[t_fdl_idx] + ncctar.sum1[t_fur_idx] - ncctar.sum1[t_ful_idx]);
                    t2 = fabs( ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_bdl_idx] - ncctar.sum2[t_bur_idx] - ncctar.sum2[t_fdr_idx] + ncctar.sum2[t_bul_idx] + ncctar.sum2[t_fdl_idx] + ncctar.sum2[t_fur_idx] - ncctar.sum2[t_ful_idx]);
                }
                t_mean = t1/len_t;
                t_std = sqrt(y_max(t2 - t1*t_mean, T1(0) ));
                T1 s1,s2;
                if(s_lx==0 && s_uy==0 && s_fz==0)
                {
                    s1 = nccsub.sum1[s_bdr_idx];
                    s2 = nccsub.sum2[s_bdr_idx];
                }
                else if(s_lx>0 && s_uy==0 && s_fz==0)
                {
                    s1 = nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_bdl_idx];
                    s2 = nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_bdl_idx];
                }
                else if(s_lx==0 && s_uy>0 && s_fz==0)
                {
                    s1 = nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_bur_idx];
                    s2 = nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_bur_idx];
                }
                else if(s_lx==0 && s_uy==0 && s_fz>0)
                {
                    s1 = nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_fdr_idx];
                    s2 = nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_fdr_idx];
                }
                else if(s_lx>0 && s_uy>0 && s_fz==0)
                {
                    s1 = nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_bdl_idx] - nccsub.sum1[s_bur_idx] + nccsub.sum1[s_bul_idx];
                    s2 = nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_bdl_idx] - nccsub.sum2[s_bur_idx] + nccsub.sum2[s_bul_idx];
                }
                else if(s_lx>0 && s_uy==0 && s_fz>0)
                {
                    s1 = nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_bdl_idx] - nccsub.sum1[s_fdr_idx] + nccsub.sum1[s_fdl_idx];
                    s2 = nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_bdl_idx] - nccsub.sum2[s_fdr_idx] + nccsub.sum2[s_fdl_idx];
                }
                else if(s_lx==0 && s_uy>0 && s_fz>0)
                {
                    s1 = nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_bur_idx] - nccsub.sum1[s_fdr_idx] + nccsub.sum1[s_fur_idx];
                    s2 = nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_bur_idx] - nccsub.sum2[s_fdr_idx] + nccsub.sum2[s_fur_idx];
                }
                else
                {
                    s1 = fabs( nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_bdl_idx] - nccsub.sum1[s_bur_idx] - nccsub.sum1[s_fdr_idx] + nccsub.sum1[s_bul_idx] + nccsub.sum1[s_fdl_idx] + nccsub.sum1[s_fur_idx] - nccsub.sum1[s_ful_idx]);
                    s2 = fabs( nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_bdl_idx] - nccsub.sum2[s_bur_idx] - nccsub.sum2[s_fdr_idx] + nccsub.sum2[s_bul_idx] + nccsub.sum2[s_fdl_idx] + nccsub.sum2[s_fur_idx] - nccsub.sum2[s_ful_idx]);
                }
                s_mean = s1/len_s;
                s_std = sqrt(y_max(s2 - s1*s_mean, T1(0) ));
                //qDebug()<< s_mean << s_std << s1 << t_mean << t_std << t1 << u << v << w; ;
                //qDebug() << "test" <<  0.5 + 0.5*(pOut.pImg[idx] - t1*t_mean)/(t_std*t_std) << u << v << w;
                // response [0, 1] (instead of [-1, 1])
                if(t_std!=0 && s_std!=0)
                {
                    //qDebug()<< "compute..." << pOut.pImg[idx];
                    pOut.pImg[idx] = 0.5 + 0.5*(pOut.pImg[idx] - t1*s_mean)/(t_std*s_std);
                    //qDebug()<< "compute...normalized" << pOut.pImg[idx];
                    if(pOut.pImg[idx]>pos->value)
                    {
                        pos->value = pOut.pImg[idx];
                        pos->x = u; pos->y = v; pos->z = w;
                        //qDebug()<< "improved..."<< pos->value;
                    }
                }
                else
                {
                    pOut.pImg[idx] = 0;
                }
            }
        }
    }
}
// second way to compute ncc with only one peak without computing sum table (slow)
// fft ncc 3d in finer scale with one peak from coarser scale 
template <class T1, class T2, class Y_IMG1, class Y_IMG2> void YImg<T1, T2, Y_IMG1, Y_IMG2> :: fftnccpns3D(Y_IMG1 pOut, Y_IMG2 pIn, T2 *sz_sub, T2 *sz_tar, T2 even_odd, bool fftwf_in_place, T1 *scale, PEAKS *pos)
{
    T2 tx=sz_tar[0], ty=sz_tar[1], tz=sz_tar[2];
    T2 sx_pad, sy_pad, sz_pad;
    sx_pad=pIn.sz[0];
    sy_pad=pIn.sz[1];
    sz_pad=pIn.sz[2];
    T2 len_pad = sx_pad*sy_pad*sz_pad;
    T2 sx, sy, sz;
    T2 sx_pad_ori;
    if(fftwf_in_place)
        sx_pad_ori = sx_pad - (2-even_odd); //2*(sx_pad/2-1);
    else
        sx_pad_ori = sx_pad;
    sx=sx_pad_ori+1-tx; // fftwf_in_place
    sy=sy_pad+1-ty;
    sz=sz_pad+1-tz;
    T2 sub_sz = sx*sy*sz;
    //cross-correlation
    fftcc3D(pOut, pIn, even_odd, fftwf_in_place);
    T2 nbbx, nbby, nbbz;
    nbbx = 2/scale[0];
    nbby = 2/scale[1];
    nbbz = 2/scale[2];
    //ncc
    pos->value = 0;
    //
    T2 pos_x = pos->x;
    T2 pos_y = pos->y;
    T2 pos_z = pos->z;
    // bounding box
    T2 w_start = pos_z - nbbx; if(w_start<0) w_start = 0;
    T2 w_end = pos_z + nbbx; if(w_end>sz_pad) w_end = sz_pad;
    T2 v_start = pos_y - nbby; if(v_start<0) v_start = 0;
    T2 v_end = pos_y + nbby; if(v_end>sy_pad) v_end = sy_pad;
    T2 u_start = pos_x - nbbz; if(u_start<0) u_start = 0;
    T2 u_end = pos_x + nbbz; if(u_end>sx_pad_ori) u_end = sx_pad_ori;
    qDebug()<< w_start << w_end << v_start << v_end << u_start << u_end;
    // computing ncc
    for(T2 w=w_start; w<w_end; w++)
    {
        T2 offset_w = w*sx_pad*sy_pad;
        for(T2 v=v_start; v<v_end; v++)
        {
            T2 offset_v = offset_w + v*sx_pad;
            for(T2 u=u_start; u<u_end; u++) // fftwf_in_place
            {
                T2 idx = offset_v + u;
                T2 t_lx, t_rx, t_uy, t_dy, t_fz, t_bz;
                T2 s_lx, s_rx, s_uy, s_dy, s_fz, s_bz;
                T1 t_std, t_mean;
                T1 s_std, s_mean;
                if(u<sx && u<tx)
                {
                    s_lx = sx-1 - u; s_rx = sx-1;
                    t_lx = sx-1; t_rx = sx-1 + u;
                }
                else if(u<sx && u>=tx)
                {
                    s_lx = sx-1 - u; s_rx = sx_pad_ori-1 - u;
                    t_lx = sx-1; t_rx = sx_pad_ori-1;
                }
                else if(u>=sx && u<tx)
                {
                    s_lx = 0; s_rx = sx-1;
                    t_lx = u; t_rx = sx-1 + u;
                }
                else if(u>=tx)
                {
                    s_lx = 0; s_rx = sx_pad_ori-1 - u;
                    t_lx = u; t_rx = sx_pad_ori-1;
                }
                else
                    printf("x direction %ld %ld %ld %ld \n", u, sx, tx, sx_pad);
                if(v<sy && v<ty)
                {
                    s_uy = sy-1 - v; s_dy = sy-1;
                    t_uy = sy-1; t_dy = sy-1 + v;
                }
                else if(v<sy && v>=ty)
                {
                    s_uy = sy-1 - v; s_dy = sy_pad-1 - v;
                    t_uy = sy-1; t_dy = sy_pad-1;
                }
                else if(v<ty && v>=sy)
                {
                    s_uy = 0; s_dy = sy-1;
                    t_uy = v; t_dy = sy-1 + v;
                }
                else if(v>=ty)
                {
                    s_uy = 0; s_dy = sy_pad-1 - v;
                    t_uy = v; t_dy = sy_pad-1;
                }
                else
                    printf("y direction %ld %ld %ld %ld \n", v, sy, ty, sy_pad);
                if(w<sz && w<tz)
                {
                    s_fz = sz-1 - w; s_bz = sz-1;
                    t_fz = sz-1; t_bz = sz-1 + w;
                }
                else if(w<sz && w>=tz)
                {
                    s_fz = sz-1 - w; s_bz = sz_pad-1 - w;
                    t_fz = sz-1; t_bz = sz_pad-1;
                }
                else if(w<tz && w>=sz)
                {
                    s_fz = 0; s_bz = sz-1;
                    t_fz = w; t_bz = sz-1 + w;
                }
                else if(w>=tz)
                {
                    s_fz = 0; s_bz = sz_pad-1 - w;
                    t_fz = w; t_bz = sz_pad-1;
                }
                else
                    printf("z direction %ld %ld %ld %ld \n", w, sz, tz, sz_pad);
                if(t_rx<t_lx || t_dy<t_uy || t_bz<t_fz || s_rx<s_lx || s_dy<s_uy || s_bz<s_fz)
                {
                    pOut.pImg[idx] = 0;
                    continue;
                }
                // overlap
                T2 len_t = (t_bz - t_fz + 1)*(t_dy - t_uy + 1)*(t_rx - t_lx + 1), len_s = (s_bz - s_fz + 1)*(s_dy - s_uy + 1)*(s_rx - s_lx + 1);
                T1 t1,t2;
                t1=0; t2=0;
                for(T2 k=t_fz; k<=t_bz; k++)
                {
                    T2 offset_k = k*sx_pad_ori*sy_pad;
                    for(T2 j=t_uy; j<=t_dy; j++)
                    {
                        T2 offset_j = offset_k + j*sx_pad_ori;
                        for(T2 i=t_lx; i<=t_rx; i++)
                        {
                            T2 idx2 = offset_j + i;
                            T1 tmp = pIn.pImg[idx2];
                            t1 += tmp;
                            t2 += tmp*tmp;
                        }
                    }
                }
                t_mean = t1/len_t;
                t_std = sqrt(max(t2 - t1*t_mean, 0));
                T1 s1,s2;
                s1=0; s2=0;
                for(T2 k=s_fz; k<=s_bz; k++)
                {
                    T2 offset_k = k*sx_pad_ori*sy_pad;
                    for(T2 j=s_uy; j<=s_dy; j++)
                    {
                        T2 offset_j = offset_k + j*sx_pad_ori;
                        for(T2 i=s_lx; i<=s_rx; i++)
                        {
                            T2 idx2 = offset_j + i;
                            T1 tmp = pOut.pImg[idx2];
                            s1 += tmp;
                            s2 += tmp*tmp;
                        }
                    }
                }
                s_mean = s1/len_s;
                s_std = sqrt(max(s2 - s1*s_mean, 0));
                //qDebug()<< s_mean << s_std << s1 << t_mean << t_std << t1 << u << v << w; ;
                //qDebug() << "test" <<  0.5 + 0.5*(pOut.pImg[idx] - t1*t_mean)/(t_std*t_std) << u << v << w;
                // response [0, 1] (instead of [-1, 1])
                if(t_std!=0 && s_std!=0)
                {
                    //qDebug()<< "compute..." << pOut.pImg[idx];
                    pOut.pImg[idx] = 0.5 + 0.5*(pOut.pImg[idx] - t1*s_mean)/(t_std*s_std);
                    //qDebug()<< "compute...normalized" << pOut.pImg[idx];
                    if(pOut.pImg[idx]>pos->value)
                    {
                        pos->value = pOut.pImg[idx];
                        pos->x = u; pos->y = v; pos->z = w;
                        //qDebug()<< "improved..."<< pos->value;
                    }
                }
                else
                {
                    pOut.pImg[idx] = 0;
                }
            }
        }
    }
}
template <class T1, class T2, class Y_IMG1, class Y_IMG2> void YImg<T1, T2, Y_IMG1, Y_IMG2> :: subpixeltranslate(Y_IMG1 pOut, T2 even_odd, bool fftwf_in_place, rPEAKS *pos)
{
    //
    T2 sx_pad, sy_pad, sz_pad;
    sx_pad=pOut.sz[0];
    sy_pad=pOut.sz[1];
    sz_pad=pOut.sz[2];
    T2 len_pad = sx_pad*sy_pad*sz_pad;
    
    T1 frac_x = pos->x - T2(pos->x);
    T1 frac_y = pos->y - T2(pos->y);
    T1 frac_z = pos->z - T2(pos->z);
    //
    //fftwf_init_threads(); //pthread
    //fftwf_plan_with_nthreads(PROCESSORS);
    fftwf_plan p;
    if(fftwf_in_place)
    {
        T2 sx_pad_ori = sx_pad - (2-even_odd); //2*(sx_pad/2-1);
        T2 len_pad_tmp = sx_pad_ori*sy_pad*sz_pad;
        p = fftwf_plan_dft_r2c_3d(sz_pad, sy_pad, sx_pad_ori, pOut.pImg, (fftwf_complex*)pOut.pImg, FFTW_ESTIMATE);
        fftwf_execute(p);
        fftwf_destroy_plan(p);
        // compute traslations
        // (a+bi)(c+di) = (ac-bd) + (bc+ad)i
        for(T2 kz=0; kz<sz_pad; kz++)
        {
            T2 offset_k = kz*sy_pad*sx_pad;
            for(T2 jy=0; jy<sy_pad; jy++)
            {
                T2 offset_j = offset_k + jy*sx_pad;
                for(T2 ix=0; ix<sx_pad; ix+=2)
                {
                    T2 idx = offset_j + ix;
                    T1 a = pOut.pImg[idx];
                    T1 b = pOut.pImg[idx+1];
                    
                    T1 phi = 2*PI*(ix*frac_x + jy*frac_y + kz*frac_z); // exp(i*phi)
                    
                    T1 c = cos(phi);
                    T1 d = sin(phi);
                    // shifts
                    pOut.pImg[idx] = a*c - b*d;
                    pOut.pImg[idx+1] = b*c + a*d;
                    
                }
            }
        }
        // obtain the phase correlation
        fftwf_complex* out_pc_sub = (fftwf_complex*)pOut.pImg;
        p = fftwf_plan_dft_c2r_3d(sz_pad, sy_pad, sx_pad_ori, out_pc_sub, pOut.pImg, FFTW_ESTIMATE);
        fftwf_execute(p);
        fftwf_destroy_plan(p);
        // cleanup
        // fftwf_cleanup_threads();
    }
    else
    {
        // need more memory
        // not support
    }
    //
    return;
}
// compute normalized cross correlation score between a single pair of images 
template <class T1, class T2, class Y_IMG1, class Y_IMG2> void YImg<T1, T2, Y_IMG1, Y_IMG2> :: cmpt_ncc3D(Y_IMG1 pOut, Y_IMG2 pIn, T2 *sz_sub, T2 *sz_tar, T2 even_odd, bool fftwf_in_place, PEAKS *&pos)
{
    //	T2 tx=sz_tar[0], ty=sz_tar[1], tz=sz_tar[2];
    //	T2 sx_pad, sy_pad, sz_pad;
    //
    //	sx_pad=pIn.sz[0];
    //	sy_pad=pIn.sz[1];
    //	sz_pad=pIn.sz[2];
    //
    //	T2 len_pad = sx_pad*sy_pad*sz_pad;
    //
    //	T2 sx, sy, sz;
    //
    //	T2 sx_pad_ori;
    //
    //	if(fftwf_in_place)
    //		sx_pad_ori = sx_pad - (2-even_odd); //2*(sx_pad/2-1);
    //	else
    //		sx_pad_ori = sx_pad;
    //
    //	sx=sx_pad_ori+1-tx; // fftwf_in_place
    //	sy=sy_pad+1-ty;
    //	sz=sz_pad+1-tz;
    //
    //	T2 sub_sz = sx*sy*sz;
    //
    //	//cross-correlation
    //	fftcc3D(pOut, pIn, even_odd, fftwf_in_place);
    //	//PEAKSLIST peakList;
    //	//fftpccc3D(pOut, pIn, even_odd, fftwf_in_place, &peakList);
    //
    //	//ncc score
    //	pos->value = 0;
    //
    //	//
    //	T2 pos_x = pos->x;
    //	T2 pos_y = pos->y;
    //	T2 pos_z = pos->z;
    //
    //	// computing ncc
    //	T2 u=pos_x, v=pos_y, w=pos_z;
    //
    //	T2 idx = w*sx_pad*sy_pad + v*sx_pad + u;
    //
    //	T2 t_lx, t_rx, t_uy, t_dy, t_fz, t_bz;
    //	T2 s_lx, s_rx, s_uy, s_dy, s_fz, s_bz;
    //
    //	T1 t_std, t_mean;
    //	T1 s_std, s_mean;
    //
    //	if(u<sx && u<tx)
    //	{
    //		s_lx = sx-1 - u; s_rx = sx-1;
    //		t_lx = sx-1; t_rx = sx-1 + u;
    //	}
    //	else if(u<sx && u>=tx)
    //	{
    //		s_lx = sx-1 - u; s_rx = sx_pad_ori-1 - u;
    //		t_lx = sx-1; t_rx = sx_pad_ori-1;
    //	}
    //	else if(u>=sx && u<tx)
    //	{
    //		s_lx = 0; s_rx = sx-1;
    //		t_lx = u; t_rx = sx-1 + u;
    //	}
    //	else if(u>=tx)
    //	{
    //		s_lx = 0; s_rx = sx_pad_ori-1 - u;
    //		t_lx = u; t_rx = sx_pad_ori-1;
    //	}
    //	else
    //		printf("x direction %ld %ld %ld %ld \n", u, sx, tx, sx_pad);
    //
    //	if(v<sy && v<ty)
    //	{
    //		s_uy = sy-1 - v; s_dy = sy-1;
    //		t_uy = sy-1; t_dy = sy-1 + v;
    //	}
    //	else if(v<sy && v>=ty)
    //	{
    //		s_uy = sy-1 - v; s_dy = sy_pad-1 - v;
    //		t_uy = sy-1; t_dy = sy_pad-1;
    //	}
    //	else if(v<ty && v>=sy)
    //	{
    //		s_uy = 0; s_dy = sy-1;
    //		t_uy = v; t_dy = sy-1 + v;
    //	}
    //	else if(v>=ty)
    //	{
    //		s_uy = 0; s_dy = sy_pad-1 - v;
    //		t_uy = v; t_dy = sy_pad-1;
    //	}
    //	else
    //		printf("y direction %ld %ld %ld %ld \n", v, sy, ty, sy_pad);
    //
    //	if(w<sz && w<tz)
    //	{
    //		s_fz = sz-1 - w; s_bz = sz-1;
    //		t_fz = sz-1; t_bz = sz-1 + w;
    //	}
    //	else if(w<sz && w>=tz)
    //	{
    //		s_fz = sz-1 - w; s_bz = sz_pad-1 - w;
    //		t_fz = sz-1; t_bz = sz_pad-1;
    //	}
    //	else if(w<tz && w>=sz)
    //	{
    //		s_fz = 0; s_bz = sz-1;
    //		t_fz = w; t_bz = sz-1 + w;
    //	}
    //	else if(w>=tz)
    //	{
    //		s_fz = 0; s_bz = sz_pad-1 - w;
    //		t_fz = w; t_bz = sz_pad-1;
    //	}
    //	else
    //		printf("z direction %ld %ld %ld %ld \n", w, sz, tz, sz_pad);
    //
    //	//
    //	if(t_rx<t_lx || t_dy<t_uy || t_bz<t_fz || s_rx<s_lx || s_dy<s_uy || s_bz<s_fz)
    //	{
    //		cout<<"Overlap region is invalid."<<endl;
    //		return;
    //	}
    //
    //	// overlap
    //	T2 len_t = (t_bz - t_fz + 1)*(t_dy - t_uy + 1)*(t_rx - t_lx + 1), len_s = (s_bz - s_fz + 1)*(s_dy - s_uy + 1)*(s_rx - s_lx + 1);
    //
    //	T1 t1,t2;
    //
    //	t1=0; t2=0;
    //	for(T2 k=t_fz; k<=t_bz; k++)
    //	{
    //		T2 offset_k = k*sx_pad_ori*sy_pad;
    //		for(T2 j=t_uy; j<=t_dy; j++)
    //		{
    //			T2 offset_j = offset_k + j*sx_pad_ori;
    //			for(T2 i=t_lx; i<=t_rx; i++)
    //			{
    //				T2 idx = offset_j + i;
    //
    //				T1 tmp = pIn.pImg[idx];
    //
    //				t1 += tmp;
    //
    //				t2 += tmp*tmp;
    //			}
    //		}
    //	}
    //
    //	t_mean = t1/len_t;
    //	t_std = sqrt(y_max(t2 - t1*t_mean, T1(0) ));
    //
    //	T1 s1,s2;
    //
    //	s1=0; s2=0;
    //	for(T2 k=s_fz; k<=s_bz; k++)
    //	{
    //		T2 offset_k = k*sx_pad_ori*sy_pad;
    //		for(T2 j=s_uy; j<=s_dy; j++)
    //		{
    //			T2 offset_j = offset_k + j*sx_pad_ori;
    //			for(T2 i=s_lx; i<=s_rx; i++)
    //			{
    //				T2 idx = offset_j + i;
    //
    //				T1 tmp = pOut.pImg[idx];
    //
    //				s1 += tmp;
    //
    //				s2 += tmp*tmp;
    //			}
    //		}
    //	}
    //
    //	s_mean = s1/len_s;
    //	s_std = sqrt(y_max(s2 - s1*s_mean, T1(0) ));
    //
    //	//qDebug()<< s_mean << s_std << s1 << t_mean << t_std << t1 << u << v << w; ;
    //	//qDebug() << "test" <<  0.5 + 0.5*(pOut.pImg[idx] - t1*t_mean)/(t_std*t_std) << u << v << w;
    //
    //	// response [0, 1] (instead of [-1, 1])
    //	if(t_std!=0 && s_std!=0)
    //	{
    //		qDebug()<< "compute..." << pOut.pImg[idx];
    //		pos->value = 0.5 + 0.5*(pOut.pImg[idx] - t1*s_mean)/(t_std*s_std);
    //		//qDebug()<< "compute...normalized" << pOut.pImg[idx];
    //	}
    T2 tx=sz_tar[0], ty=sz_tar[1], tz=sz_tar[2];
    T2 sx_pad, sy_pad, sz_pad;
    sx_pad=pIn.sz[0];
    sy_pad=pIn.sz[1];
    sz_pad=pIn.sz[2];
    T2 len_pad = sx_pad*sy_pad*sz_pad;
    T2 sx, sy, sz;
    T2 sx_pad_ori;
    if(fftwf_in_place)
        sx_pad_ori = sx_pad - (2-even_odd); //2*(sx_pad/2-1);
    else
        sx_pad_ori = sx_pad;
    sx=sx_pad_ori+1-tx; // fftwf_in_place
    sy=sy_pad+1-ty;
    sz=sz_pad+1-tz;
    T2 sub_sz = sx*sy*sz;
    int start_t = clock();
    NST<T1, T2> ncctar(pIn.sz, pIn.pImg, even_odd, fftwf_in_place, 3);
    NST<T1, T2> nccsub(pOut.sz, pOut.pImg, even_odd, fftwf_in_place, 3);
    int end_t = clock();
    qDebug() << "time consumed ... " << end_t - start_t;
    // pc and cc
    PEAKSLIST peakList;
    fftpccc3D(pOut, pIn, even_odd, fftwf_in_place, &peakList);
    //fftpatientpccc3D(pOut, pIn, even_odd, fftwf_in_place, &peakList);
    int end_tt = clock();
    qDebug() << "pccc time consumed ... " << end_tt - end_t;
    //ncc
    pos->value = 0;
    //
    T2 pos_x = pos->x;
    T2 pos_y = pos->y;
    T2 pos_z = pos->z;
    //
    T2 offset_z = sx_pad_ori*sy_pad;
    T2 u=pos_x, v=pos_y, w=pos_z;
    T2 idx = w*sx_pad*sy_pad + v*sx_pad + u;
    // z - (front, back) y - (upper, down) x - (left, right)
    T2 t_ful_idx, t_fur_idx, t_fdl_idx, t_fdr_idx, t_bul_idx, t_bur_idx, t_bdl_idx, t_bdr_idx;
    T2 s_ful_idx, s_fur_idx, s_fdl_idx, s_fdr_idx, s_bul_idx, s_bur_idx, s_bdl_idx, s_bdr_idx;
    T2 t_lx, t_rx, t_uy, t_dy, t_fz, t_bz;
    T2 s_lx, s_rx, s_uy, s_dy, s_fz, s_bz;
    T1 t_std, t_mean;
    T1 s_std, s_mean;
    if(u<sx && u<tx)
    {
        s_lx = sx-1 - u; s_rx = sx-1;
        t_lx = sx-1; t_rx = sx-1 + u;
    }
    else if(u<sx && u>=tx)
    {
        s_lx = sx-1 - u; s_rx = sx_pad_ori-1 - u;
        t_lx = sx-1; t_rx = sx_pad_ori-1;
    }
    else if(u>=sx && u<tx)
    {
        s_lx = 0; s_rx = sx-1;
        t_lx = u; t_rx = sx-1 + u;
    }
    else if(u>=tx)
    {
        s_lx = 0; s_rx = sx_pad_ori-1 - u;
        t_lx = u; t_rx = sx_pad_ori-1;
    }
    else
        printf("x direction %ld %ld %ld %ld \n", u, sx, tx, sx_pad);
    if(v<sy && v<ty)
    {
        s_uy = sy-1 - v; s_dy = sy-1;
        t_uy = sy-1; t_dy = sy-1 + v;
    }
    else if(v<sy && v>=ty)
    {
        s_uy = sy-1 - v; s_dy = sy_pad-1 - v;
        t_uy = sy-1; t_dy = sy_pad-1;
    }
    else if(v<ty && v>=sy)
    {
        s_uy = 0; s_dy = sy-1;
        t_uy = v; t_dy = sy-1 + v;
    }
    else if(v>=ty)
    {
        s_uy = 0; s_dy = sy_pad-1 - v;
        t_uy = v; t_dy = sy_pad-1;
    }
    else
        printf("y direction %ld %ld %ld %ld \n", v, sy, ty, sy_pad);
    if(w<sz && w<tz)
    {
        s_fz = sz-1 - w; s_bz = sz-1;
        t_fz = sz-1; t_bz = sz-1 + w;
    }
    else if(w<sz && w>=tz)
    {
        s_fz = sz-1 - w; s_bz = sz_pad-1 - w;
        t_fz = sz-1; t_bz = sz_pad-1;
    }
    else if(w<tz && w>=sz)
    {
        s_fz = 0; s_bz = sz-1;
        t_fz = w; t_bz = sz-1 + w;
    }
    else if(w>=tz)
    {
        s_fz = 0; s_bz = sz_pad-1 - w;
        t_fz = w; t_bz = sz_pad-1;
    }
    else
        printf("z direction %ld %ld %ld %ld \n", w, sz, tz, sz_pad);
    if(t_rx<t_lx || t_dy<t_uy || t_bz<t_fz || s_rx<s_lx || s_dy<s_uy || s_bz<s_fz)
    {
        pOut.pImg[idx] = 0;
        return;
    }
    // overlap boundary
    t_ful_idx = (t_fz-1)*offset_z + (t_uy-1)*sx_pad_ori + t_lx-1;
    t_fur_idx = (t_fz-1)*offset_z + (t_uy-1)*sx_pad_ori + t_rx;
    t_fdl_idx = (t_fz-1)*offset_z + t_dy*sx_pad_ori + t_lx-1;
    t_fdr_idx = (t_fz-1)*offset_z + t_dy*sx_pad_ori + t_rx;
    t_bul_idx = t_bz*offset_z + (t_uy-1)*sx_pad_ori + t_lx-1;
    t_bur_idx = t_bz*offset_z + (t_uy-1)*sx_pad_ori + t_rx;
    t_bdl_idx = t_bz*offset_z + t_dy*sx_pad_ori + t_lx-1;
    t_bdr_idx = t_bz*offset_z + t_dy*sx_pad_ori + t_rx;
    s_ful_idx = (s_fz-1)*offset_z + (s_uy-1)*sx_pad_ori + s_lx-1;
    s_fur_idx = (s_fz-1)*offset_z + (s_uy-1)*sx_pad_ori + s_rx;
    s_fdl_idx = (s_fz-1)*offset_z + s_dy*sx_pad_ori + s_lx-1;
    s_fdr_idx = (s_fz-1)*offset_z + s_dy*sx_pad_ori + s_rx;
    s_bul_idx = s_bz*offset_z + (s_uy-1)*sx_pad_ori + s_lx-1;
    s_bur_idx = s_bz*offset_z + (s_uy-1)*sx_pad_ori + s_rx;
    s_bdl_idx = s_bz*offset_z + s_dy*sx_pad_ori + s_lx-1;
    s_bdr_idx = s_bz*offset_z + s_dy*sx_pad_ori + s_rx;
    T2 len_t = (t_bz - t_fz + 1)*(t_dy - t_uy + 1)*(t_rx - t_lx + 1), len_s = (s_bz - s_fz + 1)*(s_dy - s_uy + 1)*(s_rx - s_lx + 1);
    T1 t1,t2;
    if(t_lx==0 && t_uy==0 && t_fz==0)
    {
        t1 = ncctar.sum1[t_bdr_idx];
        t2 = ncctar.sum2[t_bdr_idx];
    }
    else if(t_lx>0 && t_uy==0 && t_fz==0)
    {
        t1 = ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_bdl_idx];
        t2 = ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_bdl_idx];
    }
    else if(t_lx==0 && t_uy>0 && t_fz==0)
    {
        t1 = ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_bur_idx];
        t2 = ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_bur_idx];
    }
    else if(t_lx==0 && t_uy==0 && t_fz>0)
    {
        t1 = ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_fdr_idx];
        t2 = ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_fdr_idx];
    }
    else if(t_lx>0 && t_uy>0 && t_fz==0)
    {
        t1 = ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_bdl_idx] - ncctar.sum1[t_bur_idx] + ncctar.sum1[t_bul_idx];
        t2 = ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_bdl_idx] - ncctar.sum2[t_bur_idx] + ncctar.sum2[t_bul_idx];
    }
    else if(t_lx>0 && t_uy==0 && t_fz>0)
    {
        t1 = ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_bdl_idx] - ncctar.sum1[t_fdr_idx] + ncctar.sum1[t_fdl_idx];
        t2 = ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_bdl_idx] - ncctar.sum2[t_fdr_idx] + ncctar.sum2[t_fdl_idx];
    }
    else if(t_lx==0 && t_uy>0 && t_fz>0)
    {
        t1 = ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_bur_idx] - ncctar.sum1[t_fdr_idx] + ncctar.sum1[t_fur_idx];
        t2 = ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_bur_idx] - ncctar.sum2[t_fdr_idx] + ncctar.sum2[t_fur_idx];
    }
    else
    {
        t1 = fabs( ncctar.sum1[t_bdr_idx] - ncctar.sum1[t_bdl_idx] - ncctar.sum1[t_bur_idx] - ncctar.sum1[t_fdr_idx] + ncctar.sum1[t_bul_idx] + ncctar.sum1[t_fdl_idx] + ncctar.sum1[t_fur_idx] - ncctar.sum1[t_ful_idx]);
        t2 = fabs( ncctar.sum2[t_bdr_idx] - ncctar.sum2[t_bdl_idx] - ncctar.sum2[t_bur_idx] - ncctar.sum2[t_fdr_idx] + ncctar.sum2[t_bul_idx] + ncctar.sum2[t_fdl_idx] + ncctar.sum2[t_fur_idx] - ncctar.sum2[t_ful_idx]);
    }
    t_mean = t1/len_t;
    t_std = sqrt(y_max(t2 - t1*t_mean, T1(0) ));
    T1 s1,s2;
    if(s_lx==0 && s_uy==0 && s_fz==0)
    {
        s1 = nccsub.sum1[s_bdr_idx];
        s2 = nccsub.sum2[s_bdr_idx];
    }
    else if(s_lx>0 && s_uy==0 && s_fz==0)
    {
        s1 = nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_bdl_idx];
        s2 = nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_bdl_idx];
    }
    else if(s_lx==0 && s_uy>0 && s_fz==0)
    {
        s1 = nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_bur_idx];
        s2 = nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_bur_idx];
    }
    else if(s_lx==0 && s_uy==0 && s_fz>0)
    {
        s1 = nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_fdr_idx];
        s2 = nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_fdr_idx];
    }
    else if(s_lx>0 && s_uy>0 && s_fz==0)
    {
        s1 = nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_bdl_idx] - nccsub.sum1[s_bur_idx] + nccsub.sum1[s_bul_idx];
        s2 = nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_bdl_idx] - nccsub.sum2[s_bur_idx] + nccsub.sum2[s_bul_idx];
    }
    else if(s_lx>0 && s_uy==0 && s_fz>0)
    {
        s1 = nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_bdl_idx] - nccsub.sum1[s_fdr_idx] + nccsub.sum1[s_fdl_idx];
        s2 = nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_bdl_idx] - nccsub.sum2[s_fdr_idx] + nccsub.sum2[s_fdl_idx];
    }
    else if(s_lx==0 && s_uy>0 && s_fz>0)
    {
        s1 = nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_bur_idx] - nccsub.sum1[s_fdr_idx] + nccsub.sum1[s_fur_idx];
        s2 = nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_bur_idx] - nccsub.sum2[s_fdr_idx] + nccsub.sum2[s_fur_idx];
    }
    else
    {
        s1 = fabs( nccsub.sum1[s_bdr_idx] - nccsub.sum1[s_bdl_idx] - nccsub.sum1[s_bur_idx] - nccsub.sum1[s_fdr_idx] + nccsub.sum1[s_bul_idx] + nccsub.sum1[s_fdl_idx] + nccsub.sum1[s_fur_idx] - nccsub.sum1[s_ful_idx]);
        s2 = fabs( nccsub.sum2[s_bdr_idx] - nccsub.sum2[s_bdl_idx] - nccsub.sum2[s_bur_idx] - nccsub.sum2[s_fdr_idx] + nccsub.sum2[s_bul_idx] + nccsub.sum2[s_fdl_idx] + nccsub.sum2[s_fur_idx] - nccsub.sum2[s_ful_idx]);
    }
    s_mean = s1/len_s;
    s_std = sqrt(y_max(s2 - s1*s_mean, T1(0) ));
    // response [0, 1] (instead of [-1, 1])
    if(t_std!=0 && s_std!=0)
    {
        T1 tmp = pOut.pImg[idx];
        tmp = 0.5 + 0.5*(tmp - t1*s_mean)/(t_std*s_std);
        if(tmp>pos->value)
        {
            pos->value = tmp;
            pos->x = u; pos->y = v; pos->z = w;
        }
    }
}
// fusion or blending functions
//---------------------------------------------------------------------------------------------------------------------------------------------------
// func weights map for 3D linear blending
template<class T>
bool computeWeights(Y_VIM<REAL, V3DLONG, indexed_t<V3DLONG, REAL>, LUT<V3DLONG> > vim, V3DLONG i, V3DLONG j, V3DLONG k, V3DLONG tilei, T &weights)
{
    
    V3DLONG vx = vim.sz[0];
    V3DLONG vy = vim.sz[1];
    V3DLONG vz = vim.sz[2];
    V3DLONG vc = vim.sz[3];
    
    V3DLONG sz_img = vx*vy*vz;
    
    QList<T> listWeights;
    listWeights.clear();
    
    V3DLONG numtile;
    
    for(V3DLONG ii=0; ii<vim.number_tiles; ii++)
    {
        V3DLONG tile2vi_xs = vim.lut[ii].start_pos[0]-vim.min_vim[0];
        V3DLONG tile2vi_xe = vim.lut[ii].end_pos[0]-vim.min_vim[0];
        V3DLONG tile2vi_ys = vim.lut[ii].start_pos[1]-vim.min_vim[1];
        V3DLONG tile2vi_ye = vim.lut[ii].end_pos[1]-vim.min_vim[1];
        V3DLONG tile2vi_zs = vim.lut[ii].start_pos[2]-vim.min_vim[2];
        V3DLONG tile2vi_ze = vim.lut[ii].end_pos[2]-vim.min_vim[2];
        
        V3DLONG x_start = (0 > tile2vi_xs) ? 0 : tile2vi_xs;
        V3DLONG x_end = (vx-1 < tile2vi_xe) ? vx-1 : tile2vi_xe;
        V3DLONG y_start = (0 > tile2vi_ys) ? 0 : tile2vi_ys;
        V3DLONG y_end = (vy-1 < tile2vi_ye) ? vy-1 : tile2vi_ye;
        V3DLONG z_start = (0 > tile2vi_zs) ? 0 : tile2vi_zs;
        V3DLONG z_end = (vz-1 < tile2vi_ze) ? vz-1 : tile2vi_ze;
        
        x_end++;
        y_end++;
        z_end++;
        
        if(i>=x_start && i<x_end && j>=y_start && j<y_end && k>=z_start && k<z_end)
        {
            T dist2xl = fabs(T(i-x_start));
            T dist2xr = fabs(T(x_end-1-i));
            T dist2yu = fabs(T(j-y_start));
            T dist2yd = fabs(T(y_end-1-j));
            T dist2zf = fabs(T(k-z_start));
            T dist2zb = fabs(T(z_end-1-k));
            
            if( dist2xr<dist2xl ) dist2xl = dist2xr;
            if( dist2yd<dist2yu ) dist2yu = dist2yd;
            if( dist2zf>dist2zb ) dist2zf = dist2zb;
            
            listWeights.push_back(dist2xl*dist2yu*dist2zf+1);
            
            if(ii==tilei) numtile = listWeights.size()-1;
        }
        
    }
    
    if (listWeights.size()<=1)
    {
        weights=1.0;
    }
    else
    {
        T sumweights=0;
        for (int i=0; i<listWeights.size(); i++) {
            sumweights += listWeights.at(i);
        }
        weights = listWeights.at(numtile) / sumweights;
    }
    
    return true;
}
// func weights map for 3D linear blending from Tile Configuration
template<class T, class Tidx, class Y_TLUT>
bool computeWeightsTC(Y_TLUT tlut, Tidx i, Tidx j, Tidx k, Tidx tilei, T &weights)
{
    QList<T> listWeights;
    listWeights.clear();
    Tidx weighti;
    Tidx ntiles = tlut.tcList.size();
    for(Tidx ii=0; ii<ntiles; ii++)
    {
        Tidx x_start = tlut.tcList.at(ii).aBX;
        Tidx x_end = tlut.tcList.at(ii).aEX;
        Tidx y_start = tlut.tcList.at(ii).aBY;
        Tidx y_end = tlut.tcList.at(ii).aEY;
        Tidx z_start = tlut.tcList.at(ii).aBZ;
        Tidx z_end = tlut.tcList.at(ii).aEZ;
        if(i>=x_start && i<x_end && j>=y_start && j<y_end && k>=z_start && k<z_end)
        {
            T dist2xl = fabs(T(i-x_start));
            T dist2xr = fabs(T(x_end-1-i));
            T dist2yu = fabs(T(j-y_start));
            T dist2yd = fabs(T(y_end-1-j));
            T dist2zf = fabs(T(k-z_start));
            T dist2zb = fabs(T(z_end-1-k));
            if( dist2xr<dist2xl ) dist2xl = dist2xr;
            if( dist2yd<dist2yu ) dist2yu = dist2yd;
            if( dist2zf>dist2zb ) dist2zf = dist2zb;
            listWeights.push_back(dist2xl*dist2yu*dist2zf+1);
            if(ii==tilei) weighti = listWeights.size()-1;
        }
    }
    if (listWeights.size()<=1)
    {
        weights=1.0;
    }
    else
    {
        T sumweights=0;
        for (int i=0; i<listWeights.size(); i++) {
            sumweights += listWeights.at(i);
        }
        weights = listWeights.at(weighti) / sumweights;
    }
    return true;
}
// func subpixel translation using trilinear interpolation
template <class Tdata>
bool subpixshift3D(REAL *&pShift, Tdata *pImg, V3DLONG *szImg, rPEAKS pos, V3DLONG *&effectiveEnvelope)
{
    // output subpixel translated image: pShift
    // output effective envelope of the image: effectiveEnvelope[6] x,y,z
    //
    V3DLONG vx = szImg[0];
    V3DLONG vy = szImg[1];
    V3DLONG vz = szImg[2];
    V3DLONG vc = szImg[3];
    V3DLONG pagesz = vx*vy*vz;
    V3DLONG sz_img = pagesz*vc;
    //
    memset(pShift, 0.0, sizeof(Tdata)*sz_img); // filled by zeros
    //
    REAL xd = 0.0, yd = 0.0, zd = 0.0;
    V3DLONG x_start=0, y_start=0, z_start=0; // pImg
    V3DLONG xs_start=0, ys_start=0, zs_start=0; // pShift
    V3DLONG x_end=vx, y_end=vy, z_end=vz;
    if(pos.x<0)
    {
        x_start -= floor(pos.x) + 1;
        xd = -pos.x - (REAL)x_start;
        x_end -= x_start + 1;
    }
    else
    {
        xs_start += floor(pos.x) + 1;
        xd = - pos.x + (REAL)xs_start;
    }
    effectiveEnvelope[0] = xs_start;
    effectiveEnvelope[1] = x_end;
    if(pos.y<0)
    {
        y_start -= floor(pos.y) + 1;
        yd = -pos.y - (REAL)y_start;
        y_end -= y_start + 1;
    }
    else
    {
        ys_start += floor(pos.y) + 1;
        yd = - pos.y + (REAL)ys_start;
    }
    effectiveEnvelope[2] = ys_start;
    effectiveEnvelope[3] = y_end;
    if(pos.z<0)
    {
        z_start -= floor(pos.z) + 1;
        zd = -pos.z - (REAL)z_start;
        z_end -= z_start + 1;
    }
    else
    {
        zs_start += floor(pos.z) + 1;
        zd = - pos.z + (REAL)zs_start;
    }
    effectiveEnvelope[4] = zs_start;
    effectiveEnvelope[5] = z_end;
    qDebug()<<"envelop ..."<<effectiveEnvelope[0]<<effectiveEnvelope[1]<<effectiveEnvelope[2]<<effectiveEnvelope[3]<<effectiveEnvelope[4]<<effectiveEnvelope[5];
    qDebug()<<"pos ..."<<pos.x<<pos.y<<pos.z;
    //
    V3DLONG xp = 1;
    V3DLONG yp = vx;
    V3DLONG zp = vx*vy;
    REAL i1=0, i2=0, j1=0, j2=0, w1=0, w2=0;
    for(V3DLONG c=0; c<vc; c++)
    {
        V3DLONG offset_c = c*pagesz;
        for(V3DLONG k=zs_start; k<z_end; k++)
        {
            if(k-zs_start+z_start>=vz) continue;
            V3DLONG offset_ks = offset_c + k*vx*vy;
            V3DLONG offset_k = offset_c + (k-zs_start+z_start)*vx*vy;
            for(V3DLONG j=ys_start; j<y_end; j++)
            {
                if(j-ys_start+y_start>=vy) continue;
                V3DLONG offset_js = offset_ks + j*vx;
                V3DLONG offset_j = offset_k + (j-ys_start+y_start)*vx;
                for(V3DLONG i=xs_start; i<vx; i++)
                {
                    if(i-xs_start+x_start>=vx) continue;
                    V3DLONG idxs = offset_js + i;
                    V3DLONG idx = offset_j + i-xs_start+x_start;
                    i1 = (REAL)(pImg[idx])*(1.0 - zd) + (REAL)(pImg[idx+zp])*zd;
                    i2 = (REAL)(pImg[idx+yp])*(1.0 - zd) + (REAL)(pImg[idx+yp+zp])*zd;
                    j1 = (REAL)(pImg[idx+xp])*(1.0 - zd) + (REAL)(pImg[idx+xp+zp])*zd;
                    j2 = (REAL)(pImg[idx+xp+yp])*(1.0 - zd) + (REAL)(pImg[idx+xp+yp+zp])*zd;
                    w1 = i1*(1.0-yd) + i2*yd;
                    w2 = j1*(1.0-yd) + j2*yd;
                    pShift[idxs] = w1*(1.0-xd) + w2*xd;
                }
            }
        }
    }
    //
    return true;
}
#endif
