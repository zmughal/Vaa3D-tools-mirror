/* APP2_large_scale_plugin.h
 * This is a test plugin, you can use it as a demo.
 * 2014-05-12 : by Zhi Zhou
 */
 
#ifndef __APP2_LARGE_SCALE_PLUGIN_H__
#define __APP2_LARGE_SCALE_PLUGIN_H__

#include <QtGui>
#include <v3d_interface.h>

class APP2_large_scale : public QObject, public V3DPluginInterface2_1
{
	Q_OBJECT
	Q_INTERFACES(V3DPluginInterface2_1);

public:
	float getPluginVersion() const {return 1.1f;}

	QStringList menulist() const;
	void domenu(const QString &menu_name, V3DPluginCallback2 &callback, QWidget *parent);

	QStringList funclist() const ;
	bool dofunc(const QString &func_name, const V3DPluginArgList &input, V3DPluginArgList &output, V3DPluginCallback2 &callback, QWidget *parent);
};

//define a simple dialog for choose APP2 parameters and TC file
class APP2largeScaleDialog : public QDialog
    {
        Q_OBJECT

    public:
        APP2largeScaleDialog(V3DPluginCallback2 &cb, QWidget *parent)
        {
            image = 0;

            v3dhandle curwin = cb.currentImageWindow();
            if (!curwin)
            {
                v3d_msg("You don't have any images open in the main window.");
                return;
            }

            image = cb.getImage(curwin);

            if (!image)
            {
                v3d_msg("The image pointer is invalid. Ensure your data is valid and try again!");
                return;
            }

            listLandmarks = cb.getLandmark(curwin);
            if(listLandmarks.count() ==0)
            {
                v3d_msg("No markers in the current image, please select a marker.");
                return;
            }


            QGridLayout * layout = new QGridLayout();

            channel_spinbox = new QSpinBox();
            channel_spinbox->setRange(1,image->getCDim());
            channel_spinbox->setValue(1);
            cnntype_spinbox = new QSpinBox();
            cnntype_spinbox->setRange(1,3);
            cnntype_spinbox->setValue(2);
            bkgthresh_spinbox = new QSpinBox();
            bkgthresh_spinbox->setRange(-1, 255);
            bkgthresh_spinbox->setValue(10);
            lenthresh_editor = new QLineEdit(QString("").setNum(5));
            srratio_editor = new QLineEdit(QString("").setNum(3.0/9.0));
            isgsdt_checker = new QCheckBox();
            isgsdt_checker->setChecked(false);
            iswb_checker = new QCheckBox();
            iswb_checker->setChecked(false);
            b256_checker = new QCheckBox();
            b256_checker->setChecked(true);
            b_radius2Dchecker = new QCheckBox();
            b_radius2Dchecker->setChecked(true);

            block_spinbox = new QSpinBox();
            block_spinbox->setRange(1,2048);
            block_spinbox->setValue(1024);

            tc_filepath = new QLineEdit();
            openTcFile = new QPushButton(QObject::tr("..."));



            layout->addWidget(new QLabel("color channel"),0,0);
            layout->addWidget(channel_spinbox, 0,1,1,5);
            layout->addWidget(new QLabel("background_threshold \n(if set as -1, \nthen auto-thresholding)"),1,0);
            layout->addWidget(bkgthresh_spinbox, 1,1,1,5);

            QHBoxLayout * hbox1 = new QHBoxLayout();
            hbox1->addWidget(new QLabel("auto-downsample"));
            hbox1->addWidget(b256_checker);
            hbox1->addWidget(new QLabel("use GSDT"));
            hbox1->addWidget(isgsdt_checker);
            hbox1->addWidget(new QLabel("allow gap"));
            hbox1->addWidget(iswb_checker);
            hbox1->addWidget(new QLabel("radius from 2D?"));
            hbox1->addWidget(b_radius2Dchecker);

            layout->addLayout(hbox1,2,0,1,6);

            layout->addWidget(new QLabel("cnn_type"),3,0);
            layout->addWidget(cnntype_spinbox, 3,1,1,5);
            layout->addWidget(new QLabel("length_thresh"),4,0);
            layout->addWidget(lenthresh_editor, 4,1,1,5);
            layout->addWidget(new QLabel("SR_ratio"),5,0);
            layout->addWidget(srratio_editor, 5,1,1,5);
            layout->addWidget(new QLabel("block_size"),6,0);
            layout->addWidget(block_spinbox, 6,1,1,5);

            layout->addWidget(new QLabel(QObject::tr("tc file path:")),7,0);
            layout->addWidget(tc_filepath,7,1,1,4);
            layout->addWidget(openTcFile,7,5,1,1);

            QHBoxLayout * hbox2 = new QHBoxLayout();
            QPushButton * ok = new QPushButton(" ok ");
            ok->setDefault(true);
            QPushButton * cancel = new QPushButton("cancel");
            hbox2->addWidget(cancel);
            hbox2->addWidget(ok);

            layout->addLayout(hbox2,8,0,8,6);
            setLayout(layout);
            setWindowTitle(QString("Vaa3D-Neuron2 Large Scale Image Auto_tracing Based on APP2"));


            connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
            connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));

            connect(channel_spinbox, SIGNAL(valueChanged(int)), this, SLOT(update()));
            connect(cnntype_spinbox, SIGNAL(valueChanged(int)), this, SLOT(update()));
            connect(bkgthresh_spinbox, SIGNAL(valueChanged(int)), this, SLOT(update()));

            connect(lenthresh_editor, SIGNAL(selectionChanged ()), this, SLOT(update()));
            connect(srratio_editor, SIGNAL(selectionChanged ()), this, SLOT(update()));

            connect(isgsdt_checker, SIGNAL(stateChanged(int)), this, SLOT(update()));
            connect(iswb_checker, SIGNAL(stateChanged(int)), this, SLOT(update()));
            connect(b256_checker, SIGNAL(stateChanged(int)), this, SLOT(update()));
            connect(b_radius2Dchecker, SIGNAL(stateChanged(int)), this, SLOT(update()));

            connect(block_spinbox, SIGNAL(valueChanged(int)), this, SLOT(update()));
            connect(openTcFile, SIGNAL(clicked()), this, SLOT(_slots_openTcFile()));
            update();
        }

        ~APP2largeScaleDialog(){}

        public slots:
        void update()
        {
            channel = channel_spinbox->value();
            cnn_type = cnntype_spinbox->value();
            bkg_thresh = bkgthresh_spinbox->value();

            length_thresh = lenthresh_editor->text().toInt();
            SR_ratio = srratio_editor->text().toInt();

            isgsdt_checker->isChecked()? is_gsdt = 1 : is_gsdt = 0;
            iswb_checker->isChecked()? is_break_accept = 1 : is_break_accept = 0;
            b256_checker->isChecked()? b_256cube = 1 : b_256cube = 0;
            b_radius2Dchecker->isChecked() ? b_RadiusFrom2D = 1 : b_RadiusFrom2D = 0;

            block_size = block_spinbox->value();
            tcfilename = tc_filepath->text();
        }

        void _slots_openTcFile()
        {
            QFileDialog d(this);
            QString fileOpenName;
            fileOpenName = QFileDialog::getOpenFileName(0, QObject::tr("Open TC File"),
                                                        "",
                                                        QObject::tr("Supported file (*.tc)"
                                                            ));
            if(!fileOpenName.isEmpty())
            {
                tc_filepath->setText(fileOpenName);
            }
            update();

        }
    public:

        QSpinBox * channel_spinbox;
        QSpinBox * cnntype_spinbox;
        QSpinBox * bkgthresh_spinbox;
        QLineEdit * lenthresh_editor;
        QLineEdit * srratio_editor;
        QCheckBox * isgsdt_checker;
        QCheckBox * iswb_checker;
        QCheckBox * b256_checker;
        QCheckBox * b_radius2Dchecker;

        QSpinBox * block_spinbox;


        QLineEdit * tc_filepath;
        QPushButton *openTcFile;

        Image4DSimple* image;
        LandmarkList listLandmarks;
        int is_gsdt;
        int is_break_accept;
        int  bkg_thresh;
        double length_thresh;
        int  cnn_type;
        int  channel;
        double SR_ratio;
        int  b_256cube;
        int b_RadiusFrom2D;
        int block_size;

        QString tcfilename;

    };


class APP2largeScaleDialog_raw : public QDialog
    {
        Q_OBJECT

    public:
        APP2largeScaleDialog_raw(V3DPluginCallback2 &cb, QWidget *parent)
        {

            QGridLayout * layout = new QGridLayout();

            channel_spinbox = new QSpinBox();
            channel_spinbox->setRange(1,3);
            channel_spinbox->setValue(1);
            cnntype_spinbox = new QSpinBox();
            cnntype_spinbox->setRange(1,3);
            cnntype_spinbox->setValue(2);
            bkgthresh_spinbox = new QSpinBox();
            bkgthresh_spinbox->setRange(-1, 255);
            bkgthresh_spinbox->setValue(10);
            lenthresh_editor = new QLineEdit(QString("").setNum(5));
            srratio_editor = new QLineEdit(QString("").setNum(3.0/9.0));
            isgsdt_checker = new QCheckBox();
            isgsdt_checker->setChecked(false);
            iswb_checker = new QCheckBox();
            iswb_checker->setChecked(false);
            b256_checker = new QCheckBox();
            b256_checker->setChecked(true);
            b_radius2Dchecker = new QCheckBox();
            b_radius2Dchecker->setChecked(true);

            block_spinbox = new QSpinBox();
            block_spinbox->setRange(1,2048);
            block_spinbox->setValue(1024);

            raw_filepath = new QLineEdit();
            openrawFile = new QPushButton(QObject::tr("..."));

            marker_filepath = new QLineEdit();
            openmarkerFile = new QPushButton(QObject::tr("..."));



            layout->addWidget(new QLabel("color channel"),0,0);
            layout->addWidget(channel_spinbox, 0,1,1,5);
            layout->addWidget(new QLabel("background_threshold \n(if set as -1, \nthen auto-thresholding)"),1,0);
            layout->addWidget(bkgthresh_spinbox, 1,1,1,5);

            QHBoxLayout * hbox1 = new QHBoxLayout();
            hbox1->addWidget(new QLabel("auto-downsample"));
            hbox1->addWidget(b256_checker);
            hbox1->addWidget(new QLabel("use GSDT"));
            hbox1->addWidget(isgsdt_checker);
            hbox1->addWidget(new QLabel("allow gap"));
            hbox1->addWidget(iswb_checker);
            hbox1->addWidget(new QLabel("radius from 2D?"));
            hbox1->addWidget(b_radius2Dchecker);

            layout->addLayout(hbox1,2,0,1,6);

            layout->addWidget(new QLabel("cnn_type"),3,0);
            layout->addWidget(cnntype_spinbox, 3,1,1,5);
            layout->addWidget(new QLabel("length_thresh"),4,0);
            layout->addWidget(lenthresh_editor, 4,1,1,5);
            layout->addWidget(new QLabel("SR_ratio"),5,0);
            layout->addWidget(srratio_editor, 5,1,1,5);
            layout->addWidget(new QLabel("block_size"),6,0);
            layout->addWidget(block_spinbox, 6,1,1,5);

            layout->addWidget(new QLabel(QObject::tr("va3draw/raw image:")),7,0);
            layout->addWidget(raw_filepath,7,1,1,4);
            layout->addWidget(openrawFile,7,5,1,1);

            layout->addWidget(new QLabel(QObject::tr("marker file:")),8,0);
            layout->addWidget(marker_filepath,8,1,1,4);
            layout->addWidget(openmarkerFile,8,5,1,1);

            QHBoxLayout * hbox2 = new QHBoxLayout();
            QPushButton * ok = new QPushButton(" ok ");
            ok->setDefault(true);
            QPushButton * cancel = new QPushButton("cancel");
            hbox2->addWidget(cancel);
            hbox2->addWidget(ok);

            layout->addLayout(hbox2,9,0,9,6);
            setLayout(layout);
            setWindowTitle(QString("Vaa3D-Neuron2 Large Scale Image Auto_tracing Based on APP2"));


            connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
            connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));

            connect(channel_spinbox, SIGNAL(valueChanged(int)), this, SLOT(update()));
            connect(cnntype_spinbox, SIGNAL(valueChanged(int)), this, SLOT(update()));
            connect(bkgthresh_spinbox, SIGNAL(valueChanged(int)), this, SLOT(update()));

            connect(lenthresh_editor, SIGNAL(selectionChanged ()), this, SLOT(update()));
            connect(srratio_editor, SIGNAL(selectionChanged ()), this, SLOT(update()));

            connect(isgsdt_checker, SIGNAL(stateChanged(int)), this, SLOT(update()));
            connect(iswb_checker, SIGNAL(stateChanged(int)), this, SLOT(update()));
            connect(b256_checker, SIGNAL(stateChanged(int)), this, SLOT(update()));
            connect(b_radius2Dchecker, SIGNAL(stateChanged(int)), this, SLOT(update()));

            connect(block_spinbox, SIGNAL(valueChanged(int)), this, SLOT(update()));
            connect(openrawFile, SIGNAL(clicked()), this, SLOT(_slots_openrawFile()));
            connect(openmarkerFile, SIGNAL(clicked()), this, SLOT(_slots_openmarkerFile()));

            update();
        }

        ~APP2largeScaleDialog_raw(){}

        public slots:
        void update()
        {
            channel = channel_spinbox->value();
            cnn_type = cnntype_spinbox->value();
            bkg_thresh = bkgthresh_spinbox->value();

            length_thresh = lenthresh_editor->text().toInt();
            SR_ratio = srratio_editor->text().toInt();

            isgsdt_checker->isChecked()? is_gsdt = 1 : is_gsdt = 0;
            iswb_checker->isChecked()? is_break_accept = 1 : is_break_accept = 0;
            b256_checker->isChecked()? b_256cube = 1 : b_256cube = 0;
            b_radius2Dchecker->isChecked() ? b_RadiusFrom2D = 1 : b_RadiusFrom2D = 0;

            block_size = block_spinbox->value();
            rawfilename = raw_filepath->text();
            markerfilename = marker_filepath->text();

        }

        void _slots_openrawFile()
        {
            QFileDialog d(this);
            QString fileOpenName;
            fileOpenName = QFileDialog::getOpenFileName(0, QObject::tr("Open Raw File"),
                                                        "",
                                                        QObject::tr("Supported file (*.raw *.RAW *.V3DRAW *.v3draw)"
                                                            ));
            if(!fileOpenName.isEmpty())
            {
                raw_filepath->setText(fileOpenName);
            }
            update();

        }

        void _slots_openmarkerFile()
        {
            QFileDialog d(this);
            QString fileOpenName;
            fileOpenName = QFileDialog::getOpenFileName(0, QObject::tr("Open Marker File"),
                                                        "",
                                                        QObject::tr("Supported file (*.marker *.MARKER)"
                                                            ));
            if(!fileOpenName.isEmpty())
            {
                marker_filepath->setText(fileOpenName);
            }
            update();

        }
    public:

        QSpinBox * channel_spinbox;
        QSpinBox * cnntype_spinbox;
        QSpinBox * bkgthresh_spinbox;
        QLineEdit * lenthresh_editor;
        QLineEdit * srratio_editor;
        QCheckBox * isgsdt_checker;
        QCheckBox * iswb_checker;
        QCheckBox * b256_checker;
        QCheckBox * b_radius2Dchecker;

        QSpinBox * block_spinbox;


        QLineEdit * raw_filepath;
        QLineEdit * marker_filepath;
        QPushButton *openrawFile;
        QPushButton *openmarkerFile;

        Image4DSimple* image;
        LandmarkList listLandmarks;
        int is_gsdt;
        int is_break_accept;
        int  bkg_thresh;
        double length_thresh;
        int  cnn_type;
        int  channel;
        double SR_ratio;
        int  b_256cube;
        int b_RadiusFrom2D;
        int block_size;

        QString rawfilename;
        QString markerfilename;

    };

#endif
