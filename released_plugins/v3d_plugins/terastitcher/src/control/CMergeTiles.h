//------------------------------------------------------------------------------------------------
// Copyright (c) 2012  Alessandro Bria and Giulio Iannello (University Campus Bio-Medico of Rome).  
// All rights reserved.
//------------------------------------------------------------------------------------------------

/*******************************************************************************************************************************************************************************************
*    LICENSE NOTICE
********************************************************************************************************************************************************************************************
*    By downloading/using/running/editing/changing any portion of codes in this package you agree to this license. If you do not agree to this license, do not download/use/run/edit/change
*    this code.
********************************************************************************************************************************************************************************************
*    1. This material is free for non-profit research, but needs a special license for any commercial purpose. Please contact Alessandro Bria at a.bria@unicas.it or Giulio Iannello at 
*       g.iannello@unicampus.it for further details.
*    2. You agree to appropriately cite this work in your related studies and publications.
*
*       Bria, A., et al., (2012) "Stitching Terabyte-sized 3D Images Acquired in Confocal Ultramicroscopy", Proceedings of the 9th IEEE International Symposium on Biomedical Imaging.
*       Bria, A., Iannello, G., "A Tool for Fast 3D Automatic Stitching of Teravoxel-sized Datasets", submitted on July 2012 to IEEE Transactions on Information Technology in Biomedicine.
*
*    3. This material is provided by  the copyright holders (Alessandro Bria  and  Giulio Iannello),  University Campus Bio-Medico and contributors "as is" and any express or implied war-
*       ranties, including, but  not limited to,  any implied warranties  of merchantability,  non-infringement, or fitness for a particular purpose are  disclaimed. In no event shall the
*       copyright owners, University Campus Bio-Medico, or contributors be liable for any direct, indirect, incidental, special, exemplary, or  consequential  damages  (including, but not 
*       limited to, procurement of substitute goods or services; loss of use, data, or profits;reasonable royalties; or business interruption) however caused  and on any theory of liabil-
*       ity, whether in contract, strict liability, or tort  (including negligence or otherwise) arising in any way out of the use of this software,  even if advised of the possibility of
*       such damage.
*    4. Neither the name of University  Campus Bio-Medico of Rome, nor Alessandro Bria and Giulio Iannello, may be used to endorse or  promote products  derived from this software without
*       specific prior written permission.
********************************************************************************************************************************************************************************************/

#ifndef CMERGETILES_H
#define CMERGETILES_H

#include <QThread>
#include <string>
#include "vmStackedVolume.h"
#include "StackStitcher.h"
#include "CPlugin.h"
#include "src/presentation/PTabMergeTiles.h"

class terastitcher::CMergeTiles : public QThread
{
    Q_OBJECT

    private:

        /*********************************************************************************
        * Singleton design pattern: this class can have one instance only,  which must be
        * instantiated by calling static method "istance(...)"
        **********************************************************************************/
        static CMergeTiles* uniqueInstance;
        CMergeTiles() : QThread()
        {
            #ifdef TSP_DEBUG
            printf("TeraStitcher plugin [thread %d] >> CMergeTiles created\n", this->thread()->currentThreadId());
            #endif            
            reset();

        }

        //automatically called when current thread is started
        void run();

        //members
        PTabMergeTiles* pMergeTiles;
        bool resolutions[S_MAX_MULTIRES];
        int resolution_index_vaa3D;

    public:

        /*********************************************************************************
        * Singleton design pattern: this class can have one instance only,  which must be
        * instantiated by calling static method "istance(...)"
        **********************************************************************************/
        static CMergeTiles* instance()
        {
            if (uniqueInstance == NULL)
                uniqueInstance = new CMergeTiles();
            return uniqueInstance;
        }
        static void uninstance();
        ~CMergeTiles();

        void setPMergeTiles(PTabMergeTiles* handle){pMergeTiles = handle;}
        void setResolution(int index, bool enabled){resolutions[index] = enabled;}
        void setResolutionToShow(int index);

        //reset method
        void reset();

    signals:

        /*********************************************************************************
        * Carries the outcome of the operation associated to this thread.
        **********************************************************************************/
        void sendOperationOutcome(iom::exception* ex, Image4DSimple* img);
};

#endif // CMERGETILES_H
