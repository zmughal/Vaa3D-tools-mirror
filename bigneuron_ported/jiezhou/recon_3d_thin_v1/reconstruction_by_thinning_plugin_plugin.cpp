/* reconstruction_by_thinning_plugin_plugin.cpp
 * reconstruction_by_thinning_plugin
 *
 * 2015-6-1 : by Edward Hottendorf and Jie Zhou
 * Hackthon Test Plugin.
 *
 * 2015-6-3 : renamed the plugin to SimpleAxisAnalyzer (modify the pro file and the menu list, function list, usage)
 *
 * 2015-11-21 : added some simple preprocessing such as adaptive thresholding and histogram equalization
 *
 * ToDo:  Loop pruning need to be added. Big/bright soma impacts the result and should be removed first. 
 *
 */
 
#include "v3d_message.h"
#include <vector>
#include <iostream>
#include "basic_surf_objs.h"

//itk headers
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"

#include "itkConnectedThresholdImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkBinaryThinningImageFilter3D.h"

#include "Point.h"
#include "pixPoint.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <queue>
#include <ctime>

#include "reconstruction_by_thinning_plugin_plugin.h"
Q_EXPORT_PLUGIN2(reconstruction_by_thinning_plugin, reconstruction_by_thinning_plugin);

using namespace std;

struct input_PARA
{
    QString inimg_file;
    V3DLONG channel = 1;
    int threshold = 0 ;
    int treeNum = 1;
};

void reconstruction_func(V3DPluginCallback2 &callback, QWidget *parent, input_PARA &PARA, bool bmenu);

//added functions here
itk::Image<signed short, (unsigned) 3>* createITKImage(unsigned char *datald, int *in_sz);
void reconstructionThinning(itk::Image<signed short, (unsigned) 3> *input, QString qoutputfilename, int reconstructionTh, int treeNum);
vector<pixPoint*> visitSkeleton(itk::Image<signed short, (unsigned)3> *image, pixPoint* root, bool ***);
void trueAnalyze(itk::Image<signed short, (unsigned)3> *image, QString outfilename, int treeNum);
signed short getPixel(itk::Image<signed short, (unsigned)3> *image, int x, int y, int z);
void outputSWC(vector<pixPoint*> & Points, QString outfilename, long);
signed short getPixel(itk::Image<signed short, (unsigned)3> *image, int x, int y, int z);
void getNeighbors(itk::Image<signed short, (unsigned)3> *image, vector<pixPoint*> & neigh, pixPoint* p);
int numberOfNeighbors(itk::Image<signed short, (unsigned)3> *image, int xCoord, int yCoord, int zCoord);
bool *** resetVisited(itk::Image<signed short, (unsigned)3> *image);

//This method is from adaptive thresholdoing plugin. Add a binarization process for some images in BigNeuron gold standard set. 11/2015
void BinaryProcess(unsigned char *apsInput, unsigned char * aspOutput, V3DLONG iImageWidth, V3DLONG iImageHeight, V3DLONG iImageLayer, V3DLONG h, V3DLONG d)
{
        V3DLONG i, j,k,n,count;
        double t, temp;

        V3DLONG mCount = iImageHeight * iImageWidth;
        for (i=0; i<iImageLayer; i++)
        {
                for (j=0; j<iImageHeight; j++)
                {
                        for (k=0; k<iImageWidth; k++)
                        {
                                V3DLONG curpos = i * mCount + j*iImageWidth + k;
                                V3DLONG curpos1 = i* mCount + j*iImageWidth;
                                V3DLONG curpos2 = j* iImageWidth + k;
                                temp = 0;
                                count = 0;
                                for(n =1 ; n <= d  ;n++)
                                {
                                        if (k>h*n) {temp += apsInput[curpos1 + k-(h*n)]; count++;}
                                        if (k+(h*n)< iImageWidth) { temp += apsInput[curpos1 + k+(h*n)]; count++;}
                    if (j>h*n) {temp += apsInput[i* mCount + (j-(h*n))*iImageWidth + k]; count++;}//
                                        if (j+(h*n)<iImageHeight) {temp += apsInput[i* mCount + (j+(h*n))*iImageWidth + k]; count++;}//
                                        if (i>(h*n)) {temp += apsInput[(i-(h*n))* mCount + curpos2]; count++;}//
                                        if (i+(h*n)< iImageLayer) {temp += apsInput[(i+(h*n))* mCount + j* iImageWidth + k ]; count++;}
                                }
                                t =  apsInput[curpos]-temp/(count);
                                aspOutput[curpos]= (t > 0)? t : 0;
                        }
                }
        }
}

//v3d preprocessing method for histogram equalization
bool hist_eq_uint8(unsigned char * data1d, V3DLONG len)
{
        if (!data1d || len<=0)
        {
                printf("The input parameters are invalid in hist_eq_uint8().\n");
                return false;
        }
        
        V3DLONG NBIN=256;
        V3DLONG *h = new V3DLONG [NBIN]; 
        double *c = new double [NBIN]; 
        if (!h)
        {
                printf("Fail to allocate memory in hist_eq_uint8().\n");
                return false;
        }
        
        V3DLONG i;
        
        for (i=0;i<NBIN;i++) h[i]=0;
        
        for (i=0;i<len; i++)
        {
                h[data1d[i]]++;
        }
        
        c[0]=h[0];
        for (i=1;i<NBIN;i++) c[i] = c[i-1]+h[i];
        for (i=0;i<NBIN;i++) {c[i] /= c[NBIN-1]; c[i] *= (NBIN-1);}
        
        for (i=0;i<len;i++)
        {
                data1d[i] = c[data1d[i]];
        }
        
        if (c) {delete []c; c=0;}
        if (h) {delete []h; h=0;}
        return true;
}

bool hist_eq_range_uint8(unsigned char * data1d, V3DLONG len, unsigned char lowerbound, unsigned char upperbound) //only eq the [lowerbound,upperbound]
{
        if (!data1d || len<=0)
        {
                printf("The input parameters are invalid in hist_eq_uint8().\n");
                return false;
        }
        
        if (lowerbound>upperbound) 
        {
                unsigned char tmp=lowerbound; lowerbound=upperbound; upperbound=tmp;
        }
        
        V3DLONG NBIN=256;
        V3DLONG *h = new V3DLONG [NBIN]; 
        double *c = new double [NBIN]; 
        if (!h)
        {
                printf("Fail to allocate memory in hist_eq_uint8().\n");
                return false;
        }
        
        V3DLONG i;
        
        for (i=0;i<NBIN;i++) h[i]=0;
        
        for (i=0;i<len; i++)
        {
                h[data1d[i]]++;
        }
        
        c[lowerbound]=h[lowerbound];
        for (i=lowerbound+1;i<=upperbound;i++) c[i] = c[i-1]+h[i];
        double range = upperbound-lowerbound;
        for (i=lowerbound;i<=upperbound;i++) {c[i] /= c[upperbound]; c[i] *= range; c[i] += lowerbound;}
        
        for (i=0;i<len;i++)
        {
                if (data1d[i]>=lowerbound && data1d[i]<=upperbound)
                        data1d[i] = c[data1d[i]];
        }
        
        if (c) {delete []c; c=0;}
        if (h) {delete []h; h=0;}
        return true;
}


//******** main methods for the axis analyzer start here ********/
typedef itk::Image<signed short, 3> ImageType;
ImageType::Pointer globalImage = ImageType::New();


QStringList reconstruction_by_thinning_plugin::menulist() const
{
	return QStringList() 
        <<tr("medial_axis_analysis")
		<<tr("about");
}

QStringList reconstruction_by_thinning_plugin::funclist() const
{
	return QStringList()
        <<tr("medial_axis_analysis")
		<<tr("help");
}

void reconstruction_by_thinning_plugin::domenu(const QString &menu_name, V3DPluginCallback2 &callback, QWidget *parent)
{
    if (menu_name == tr("medial_axis_analysis"))
	{
        bool bmenu = true;
        input_PARA PARA;

        //input
         bool ok1, ok2;

        PARA.threshold = QInputDialog::getInteger(parent, "Window X ",
                                       "Enter threshold (voxel > threshold considered as foreground. Default 0):",
                                       0, 0, 255, 1, &ok1);

        if (!ok1) return;  //have problem getting threshold

        PARA.treeNum = QInputDialog::getInteger(parent, "Window X ",
                                       "Enter number of tree you would like to reconstruct (Default 10):",
                                       10, 1, 15, 1, &ok2);

        if (!ok2) return;  //have problem getting threshold

        reconstruction_func(callback,parent,PARA,bmenu);
        
	}
	else
	{
        v3d_msg(tr("NeuronAxisAnalyser -- Input: Gray-level Image (or Binary Image); Output: Reconstructed swc\n"
            "Developed by Jie Zhou and Edward Hottendorf, 2015-6-1"));
	}
}

bool reconstruction_by_thinning_plugin::dofunc(const QString & func_name, const V3DPluginArgList & input, V3DPluginArgList & output, V3DPluginCallback2 & callback,  QWidget * parent)
{
    if (func_name == tr("medial_axis_analysis"))
	{
        bool bmenu = false;
        input_PARA PARA;

        vector<char*> * pinfiles = (input.size() >= 1) ? (vector<char*> *) input[0].p : 0;
        vector<char*> * pparas = (input.size() >= 2) ? (vector<char*> *) input[1].p : 0;
        vector<char*> infiles = (pinfiles != 0) ? * pinfiles : vector<char*>();
        vector<char*> paras = (pparas != 0) ? * pparas : vector<char*>();

        if(infiles.empty())
        {
            fprintf (stderr, "Need input image. \n");
            return false;
        }
        else
            PARA.inimg_file = infiles[0];
        int k=0;
        PARA.threshold = (paras.size() >= k+1) ? atoi(paras[k]) : 0;  k++;
        std::cout<< "PARA.threshold: " << PARA.threshold << endl;
        PARA.treeNum = (paras.size() >= k+1) ? atoi(paras[k]) : 1;  k++;
        PARA.channel = (paras.size() >= k+1) ? atoi(paras[k]) : 1;  k++;


        reconstruction_func(callback,parent,PARA,bmenu);
	}
    else if (func_name == tr("help"))
    {

        ////HERE IS WHERE THE DEVELOPERS SHOULD UPDATE THE USAGE OF THE PLUGIN

        printf("**** Usage of axis analyzer plugin **** \n");
        printf("vaa3d -x SimpleAxisAnalyzer -f medial_axis_analysis -i <inimg_file> -p <parameters>\n");
        printf("inimg_file       The input image\n");
        printf("threshold        Threshold by binarization (default 0).\n");
        printf("numberOfTrees    Number of trees to include in reconstruction (default 1)");
        printf("channel          Data channel for tracing. Start from 1 (default 1).\n");

        printf("outswc_file      Will be named automatically based on the input image file name, so you don't have to specify it.\n\n");

     }
     else return false;

    return true;
}

void reconstruction_func(V3DPluginCallback2 &callback, QWidget *parent, input_PARA &PARA, bool bmenu)
{
    unsigned char* data1d = 0;
    V3DLONG N,M,P,sc,c;
    V3DLONG in_sz[4];
    if(bmenu)
    {
        v3dhandle curwin = callback.currentImageWindow();
        if (!curwin)
        {
            QMessageBox::information(0, "", "You don't have any image open in the main window.");
            return;
        }

        Image4DSimple* p4DImage = callback.getImage(curwin);

        if (!p4DImage)
        {
            QMessageBox::information(0, "", "The image pointer is invalid. Ensure your data is valid and try again!");
            return;
        }


        data1d = p4DImage->getRawData();
        N = p4DImage->getXDim();
        M = p4DImage->getYDim();
        P = p4DImage->getZDim();
        sc = p4DImage->getCDim();

        bool ok1;

        if(sc==1)
        {
            c=1;
            ok1=true;
        }
        else
        {
            c = QInputDialog::getInteger(parent, "Channel",
                                             "Enter channel NO:",
                                             1, 1, sc, 1, &ok1);
        }

        if(!ok1)
            return;

        in_sz[0] = N;
        in_sz[1] = M;
        in_sz[2] = P;
        in_sz[3] = sc;


        PARA.inimg_file = p4DImage->getFileName();
    }
    else
    {
        int datatype = 0;
        if (!simple_loadimage_wrapper(callback,PARA.inimg_file.toStdString().c_str(), data1d, in_sz, datatype))
        {
            fprintf (stderr, "Error happens in reading the subject file [%s]. Exit. \n",PARA.inimg_file.toStdString().c_str());
            return;
        }
        if(PARA.channel < 1 || PARA.channel > in_sz[3])
        {
            fprintf (stderr, "Invalid channel number. \n");
            return;
        }
        N = in_sz[0];
        M = in_sz[1];
        P = in_sz[2];
        sc = in_sz[3];
        c = PARA.channel;
    }

    //main neuron reconstruction code

    //// THIS IS WHERE THE DEVELOPERS SHOULD ADD THEIR OWN NEURON TRACING CODE

    //1. convert vaa3D image to itk image
    int imageSize[3];
    imageSize[0] = N;
    imageSize[1] = M;
    imageSize[2] = P;

    cout << "width: "  << N << endl;
    cout << "height: "  << M << endl;
    cout << "depth: "  << P << endl;

    // * Note by J Zhou June 3 2015
    // * Vaa3D appears having issue reading a signed int image of only 0 and 1 which was the output skeleton of the thinning algorithm of itk.
    // *  But it is ok as long as the plugin starts directly from an unsigned image file.
  
    // * Nov 20 2015: ORNL Hackthon: add some preprocessing/thresholding method since some images in BigNeuron gold standard are noisy and not binarized.

    if (PARA.threshold == 0)
    {  
      //adaptive thresholding is not suitable for image with big soma since it will leave hole in the middle
      V3DLONG h = 5, d = 3;
      unsigned char *pData = NULL;
      try
      {
         pData = (unsigned char *) (new unsigned char [N*M*P]);
      }catch(...)
      { v3d_msg("Fail to allocate memory when applying adaptive thresholding for simple axis anlyzer.",0);
        if (pData) { delete []pData; pData = NULL;}
        return;
      }
        BinaryProcess(data1d, pData, N, M, P, h, d);
        delete[] data1d; 
        data1d = pData;
        v3d_msg("Done thresholding", 0);       
       
       //stat of thresholed forground pixels to see if making sense
       long num =0;         
       for(int iz=0; iz<P; iz++) {
         int offsetk = iz*M*N;
         for(int iy =0; iy < M; iy++) {
           int offsetj = iy*N;
           for(int ix = 0; ix < N; ix++) {
              int datavalue = data1d[offsetk+offsetj+ix];
              if (datavalue > 0) num ++;
           }
         }
       }
       v3d_msg(QString("Total percentage of foreground pixels after adaptive thresholding:%1").arg(((float)num)/(N*M*P)),0);

     //enhance the histogram before apply binarization.
     //This avoids have broken holes in the connected structure.
     hist_eq_range_uint8(data1d, N*M*P, 10, 255);
     v3d_msg(QString("Histogram equalization done"), 0);
    }
        
    //iterative histogram-based thresholding 
    if (PARA.threshold == 999 || PARA.threshold == 0) 
    {
         long num = 0;
         long sum = 0;
         for(int iz = 0; iz < P; iz++)
         {
           int offsetk = iz*M*N;
           for(int iy = 0; iy < M; iy++)
           {
             int offsetj = iy*N;
             for(int ix = 0; ix < N; ix++)
             {

              int PixelValue =  data1d[offsetk + offsetj + ix];
              if (PixelValue > 0)
              {
                num++;
                sum += PixelValue;
                if (num < 10)
                cout << ix << ":" << iy << ":" << iz << " ";

              }
             } 
            }
          }
          int meanv = sum/num;
          long sum1, num1, sum2, num2;   
          int iterationnum = 2;// find the valley in gray level historgram, keep small to avoid under segmentation
          for(int it =0; it < iterationnum; it++)
          {
            sum1=0, num1=0;
            sum2=0, num2=0;
            for(long iz = 0; iz < P; iz++)
            {   
               long offsetk = iz*M*N;
               for(long iy = 0; iy < M; iy++)
               {
                 long offsetj = iy*N;
                 for(long ix = 0; ix < N; ix++)
                 {
                   long idx =  offsetk + offsetj + ix;
                   if (data1d[idx] > meanv)
                   {  sum1 += data1d[idx]; num1++; }
                   else
                   {   sum2 += data1d[idx]; num2++;}
                }  
              }   
            }  
            //adjust new threshold
           meanv = (int) (0.5*(sum1/num1+sum2/num2));
         }
         //meanv = meanv - 10; //adjust low due to dark image to avoid undertrace
         std::cout << "\nmean adjusted:" << meanv <<std::endl;

         v3d_msg(QString("Total foreground points:%1").arg(sum1), 0);
         v3d_msg(QString("Total percentage of foreground points after binarization:%1").arg(((float)num1)/(N*M*P)),0); 
   
         PARA.threshold = meanv;
    }//end of thresholding!!

    //itk::Image<signed short, (unsigned) 3> *input;
    itk::Image<signed short, (unsigned) 3> *input = createITKImage(data1d, imageSize);


    //2. call Reconstruction3D by passing the itkimage

    QString swc_name = PARA.inimg_file + "_axis_analyzer.swc";

    reconstructionThinning(input, swc_name, PARA.threshold, PARA.treeNum);

    if(!bmenu)
    {
        if(data1d) {delete []data1d; data1d = 0;}
    }
    v3d_msg(QString("Now you can drag and drop the generated swc fle [%1] into Vaa3D.").arg(swc_name.toStdString().c_str()),bmenu);

}

//the logic is similar as those in the main of Reconstruction3D()
void reconstructionThinning(itk::Image<signed short, (unsigned) 3> *input, QString qoutputfilename, int reconstructionTh, int treeNumber)
{
     typedef itk::Image< signed short, (unsigned)3> ImageType;
    // Define the thinning filter
    typedef itk::BinaryThinningImageFilter3D< ImageType, ImageType > ThinningFilterType;

    ThinningFilterType::Pointer thinningFilter = ThinningFilterType::New();
    thinningFilter->threshold = reconstructionTh; //threshold to process images of non-zero background
    thinningFilter->SetInput(input);

    v3d_msg(QString("Start thinning with reconstruction threshold %1 ...").arg(reconstructionTh),0 );
    clock_t begin = clock();


    thinningFilter->Update();

    v3d_msg("after Thinning is done ", 0);
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

    v3d_msg(QString("time used for thinning in secs  %1").arg(elapsed_secs), 0);

    //call true analyze which outputs swc file
    trueAnalyze(thinningFilter->GetOutput(), qoutputfilename, treeNumber);

}


//ImageOperation: convert Vaa3D to itk image
itk::Image<signed short, (unsigned) 3>* createITKImage(unsigned char *datald, int *in_sz)
{
             typedef itk::Image<signed short, 3> ImageType; //signed short is required for the thinning algorithm
             int SN = in_sz[0];
             int SM = in_sz[1];
             int SZ = in_sz[2];

             ImageType::Pointer I  = ImageType::New();

             ImageType::SizeType size;
             size[0] = SN;
             size[1] = SM;
             size[2] = SZ;

             ImageType::IndexType idx;
             idx.Fill(0);
             ImageType::RegionType region;
             region.SetSize( size );
             region.SetIndex( idx );

             I->SetRegions(region);
             I->Allocate();
             I->FillBuffer(0);

             signed short PixelValue;
             for(int iz = 0; iz < SZ; iz++)
             {
                 int offsetk = iz*SM*SN;
                 for(int iy = 0; iy < SM; iy++)
                 {
                    int offsetj = iy*SN;
                    for(int ix = 0; ix < SN; ix++)
                    {

                       PixelValue = (signed short) datald[offsetk + offsetj + ix];
                       itk::Index<3> indexX;
                       indexX[0] = ix;
                       indexX[1] = iy;
                       indexX[2] = iz;
                       I->SetPixel(indexX, PixelValue);
                    }
                }
             }

             //return the filled image pointer
             globalImage = I; //optional (testing purpsoe only)

             return I;


}




void trueAnalyze(itk::Image<signed short, (unsigned)3> *image, QString outfilename, int treeNumber){


        typedef itk::Image< signed short, (unsigned)3> ImageType;
        typedef signed short PixelType;

        vector <pixPoint*> endPoints;

        vector <Point*> points;

        vector<vector<pixPoint*> > skeletonPoints;
        //num of trees can be found by calling skeletonPoints.size()
        //num of pixels in a tree can be found by calling skeletonPoints[sumNum].size();

        //This sets the region to the entire image
        ImageType::RegionType region = image->GetLargestPossibleRegion();

        //returns a sizetype data type that is an array of three elements
        ImageType::SizeType size = region.GetSize();
        int width = size[0];
        int height = size[1];
        int depth = size[2];


        signed short crntPoint;

        for (int x = 0; x < width; x++)
                for (int y = 0; y < height; y++)
                        for (int z = 0; z < depth; z++)
                        {

                                crntPoint = getPixel(image, x, y, z);

                                        if (crntPoint != 0)
                                         {
                                             points.push_back(new Point(x, y, z));
                                          }
                        }


        v3d_msg(QString("in trueAnalyze: number of fg points: %1").arg(points.size()), 0);
        for (int i = 0; i < points.size(); i++)
        {
             if (numberOfNeighbors(image, points[i]->x, points[i]->y, points[i]->z) == 1){
            //int neighbor = numberOfNeighbors(globalImage, points[i]->x, points[i]->y, points[i]->z);
            //cout << points[i]->x << " " << points[i]->y << " " << points[i]->z << " " << neighbor << endl;
            //if (neighbor == 1){
                        endPoints.push_back(new pixPoint(points[i]->x, points[i]->y, points[i]->z));

                }
        }


        v3d_msg(QString("in trueAnalyze: number of end points in skeleton: %1").arg(endPoints.size()), 0);
        bool *** visited = resetVisited(image);

        for (int i = 0; i < endPoints.size(); i++)
                if (visited[endPoints[i]->x][endPoints[i]->y][endPoints[i]->z] != true)
                      skeletonPoints.push_back(visitSkeleton(image, endPoints[i], visited));

        v3d_msg(QString("Number of total trees we get:  %1").arg(skeletonPoints.size()), 0);

        //bubble sort ascending order so the last tree
        int* indexNum = new int[skeletonPoints.size()];
        for (int i = 0; i < skeletonPoints.size(); i++)
        {
                indexNum[i] = i;
        }


        int i, j;
        int temp;
        bool swapped = true;

        for (i = (skeletonPoints.size() - 1); i >= 0 && swapped; i--)
        {
                swapped = false;

                for (j = 1; j <= i; j++)
                {
                        if (skeletonPoints[indexNum[j - 1]].size() > skeletonPoints[indexNum[j]].size())
                        {
                                temp = indexNum[j - 1];
                                indexNum[j - 1] = indexNum[j];
                                indexNum[j] = temp;
                                swapped = true;
                        }
                }
        }


        vector<pixPoint*> Points = skeletonPoints[indexNum[skeletonPoints.size() - 1 ]];

        /*
        for (int i = 0; i < Points.size(); i++){
                        cout << Points[i]->visitNum << ' ' << 7 << ' ' << Points[i]->x << ' ' << Points[i]->y << ' ' << Points[i]->z << ' ' << 1 << ' ' << Points[i]->parent << endl;
                }
         */

        //output certain number of largest trees to a swc file, will be a parameter

        //int treeNumerb = PARA.treeNum;
        if (treeNumber >  skeletonPoints.size())
               treeNumber = skeletonPoints.size();

        long startVisitNum = 0;
        for (int i = 1; i<= treeNumber; i++)
        {
          outputSWC(skeletonPoints[indexNum[skeletonPoints.size() - i ]], outfilename, startVisitNum); //currently outputing the largest tree
          startVisitNum += skeletonPoints[indexNum[skeletonPoints.size() - i ]].size();
        }


        points.clear();
        endPoints.clear();

        delete  visited;
}//end trueAnalyze



vector<pixPoint*> visitSkeleton(itk::Image<signed short, (unsigned)3> *image, pixPoint* root, bool *** visited){

        pixPoint* crntPoint = new pixPoint(root->x, root->y, root->z);
        int num = 1;

        visited[crntPoint->x][crntPoint->y][crntPoint->z] = true;
        crntPoint->visitNum = num;
        crntPoint->parent = -1;

        queue<pixPoint*> pntQ;
        pntQ.push(crntPoint);

        vector<pixPoint*> crntSkelPoints;
        crntSkelPoints.push_back(crntPoint);

        vector<pixPoint*> neighbor;


        while (!pntQ.empty()){

                getNeighbors(image, neighbor, pntQ.front()); //neighbor passed in by reference
                //all of the neighbors will only have x, y, z at this point

                for (int i = 0; i < neighbor.size(); i++)
                {
                        if (visited[neighbor[i]->x][neighbor[i]->y][neighbor[i]->z] == false)
                        {
                                num++;
                                visited[neighbor[i]->x][neighbor[i]->y][neighbor[i]->z] = true;
                                neighbor[i]->visitNum = num;
                                neighbor[i]->parent = pntQ.front()->visitNum;


                                pntQ.push(neighbor[i]);
                                crntSkelPoints.push_back(neighbor[i]);

                        }
                }

                neighbor.clear();
                pntQ.pop();

        }


        return crntSkelPoints;

}//end visitSkeleton


bool *** resetVisited(itk::Image<signed short, (unsigned)3> *image){

        typedef itk::Image< signed short, (unsigned)3> ImageType;
        typedef signed short PixelType;
        //This sets the region to the entire image
        ImageType::RegionType region = image->GetLargestPossibleRegion();

        //returns a sizetype data type that is an array of three elements
        ImageType::SizeType size = region.GetSize();

        int width = size[0];
        int height = size[1];
        int depth = size[2];

        // Reset visited variable
        bool *** visited = new bool**[width];
        for (int i = 0; i < width; i++){
                visited[i] = new bool*[height];
                for (int u = 0; u < height; u++){
                        visited[i][u] = new bool[depth];
                }
        }

        for (int i = 0; i < width; i++)
                for (int j = 0; j < height; j++)
                        for (int k = 0; k < depth; k++)
                                visited[i][j][k] = false;

        return visited;
}//end resetVisited



int numberOfNeighbors(itk::Image<signed short, (unsigned)3> *image, int xCoord, int yCoord, int zCoord){

        int n = 0;

        for (int x = -1; x <= 1; x++)
                for (int y = -1; y <= 1; y++)
                        for (int z = -1; z <= 1; z++)
                        {

                                if (x == 0 && y == 0 && z == 0)
                                {
                                    //int self = getPixel(globalImage, xCoord + x, yCoord + y, zCoord + z);
                                    //cout << "self" << " " << self << " ";

                                        continue;
                                }

                               if (getPixel(image, xCoord + x, yCoord + y, zCoord + z) != 0)
                               // if (getPixel(globalImage, xCoord + x, yCoord + y, zCoord + z) != 0)
                                        n++;

                        }
        return n;
}//end numberOfNeighbors




void getNeighbors(itk::Image<signed short, (unsigned)3> *image, vector<pixPoint*> & neigh, pixPoint* p){



        for (int x = -1; x <= 1; x++)
                for (int y = -1; y <= 1; y++)
                        for (int z = -1; z <= 1; z++)
                        {

                                if (x == 0 && y == 0 && z == 0)
                                        continue;

                                if (getPixel(image, p->x + x, p->y + y, p->z + z) != 0)
                                {
                                        neigh.push_back(new pixPoint(p->x + x, p->y + y, p->z + z));
                                }

                        }

}//end getNeighbors




signed short getPixel(itk::Image<signed short, (unsigned)3> *image, int x, int y, int z){
        typedef itk::Image< signed short, (unsigned)3> ImageType;
        typedef signed short PixelType;
        //This sets the region to the entire image
        ImageType::RegionType region = image->GetLargestPossibleRegion();
        //ImageType::RegionType region = globalImage->GetLargestPossibleRegion();

        //returns a sizetype data type that is an array of three elements
        ImageType::SizeType size = region.GetSize();

        int width = size[0];
        int height = size[1];
        int depth = size[2];

        typedef itk::Image< signed short, 3 > ImageType;
        ImageType::IndexType pixelIndex;
        pixelIndex[0] = x;
        pixelIndex[1] = y;
        pixelIndex[2] = z;

        if (x >= 0 && x < width && y >= 0 && y < height && z >= 0 && z < depth)
             return image->GetPixel(pixelIndex);
        else
                return 0;
}//end getpixel


void outputSWC(vector<pixPoint*> & Points, QString qoutputfilename, long startVisitNum){


       const char *outfilename = qoutputfilename.toStdString().c_str();
       cout << "filename converted from QString" << outfilename << endl;

       ofstream outFile;
       if (startVisitNum == 0) //first tree
       {
           outFile.open(outfilename, ios::trunc);
           outFile << "#name reconstruction3d \n#comment \n##n,type,x,y,z,radius,parent\n";
       }
       else //subsequent trees
           outFile.open(outfilename, ios::app);

       for (int i = 0; i < Points.size(); i++)
       {
           if (Points[i]->parent == -1)  //root of a separate tree
              outFile << Points[i]->visitNum + startVisitNum << ' ' << 7 << ' ' << Points[i]->x << ' ' << Points[i]->y << ' ' << Points[i]->z << ' ' << 1 << ' ' << -1 << endl;

           else
             outFile << Points[i]->visitNum + startVisitNum << ' ' << 7 << ' ' << Points[i]->x << ' ' << Points[i]->y << ' ' << Points[i]->z << ' ' << 1 << ' ' << Points[i]->parent + startVisitNum << endl;
       }

        outFile.close();
        cout << outfilename << " Successfully written to swc file.";

}//end outputSWC


