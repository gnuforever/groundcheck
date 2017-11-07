#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "comm_def.h"
#include "networkthread.h"
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QFileDialog>
#include <QObject>
#include <QTextStream>
#include <QByteArray>
#include <QtConcurrent/QtConcurrent>
#include <QDebug>
#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <opencv/cv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

CAM_CFG_FILE gCamCfgFile[CAM_NUM_MAX] = {
    {0, CAM0_CFG_FILE},
    {1, CAM1_CFG_FILE},
    {2, CAM2_CFG_FILE},
    {3, CAM3_CFG_FILE}
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QImage ScaleImage2Label(QImage qImage, QLabel* qLabel)
{
    QImage qScaledImage;
    QSize qImageSize = qImage.size();
    QSize qLabelSize = qLabel->size();

    double dWidthRatio = 1.0*qImageSize.width() / qLabelSize.width();
    double dHeightRatio = 1.0*qImageSize.height() / qLabelSize.height();
    if(dWidthRatio>dHeightRatio){
        qScaledImage = qImage.scaledToWidth(qLabelSize.width());
    }else{
        qScaledImage = qImage.scaledToHeight(qLabelSize.height());
    }
    return qScaledImage;
}

QImage *IplImageToGrayQImage(const IplImage *img)
{
    QImage *image;
    uchar *imgData=(uchar *)img->imageData;
    image=new QImage(imgData, img->width, img->height, QImage::Format_Grayscale8);
    return image;
}

QImage *IplImageToQImage(IplImage *img)
{
    QImage *image;
    cvCvtColor(img,img,CV_BGR2RGB);
    uchar *imgData=(uchar *)img->imageData;
    image=new QImage(imgData,img->width,img->height,QImage::Format_RGB888);
    return image;
}

void QImageToIplImage(const QImage * qImage, IplImage **IplImageBuffer)
{
    int width = qImage->width();
    int height = qImage->height();
    CvSize Size;
    Size.height = height;
    Size.width = width;
    IplImage *pIplImage = *IplImageBuffer;
    //printf("%s:%d height = %d, width = %d\n", __FUNCTION__, __LINE__, height, width);
    QString h = QString::number(height, 10);
    QString w = QString::number(width, 10);
    qDebug()<<"height =" + h.toLatin1() + "width = " + w.toLatin1()<<endl;
#if 0
    if(NULL != IplImageBuffer){
        cvReleaseImage(&IplImageBuffer);
    }
#endif
    pIplImage = cvCreateImage(Size, IPL_DEPTH_8U, 3);
    h = QString::number(pIplImage->height, 10);
    w = QString::number(pIplImage->width, 10);
    qDebug()<<"h =" + h.toLatin1() + "w = " + w.toLatin1()<<endl;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QRgb rgb = qImage->pixel(x, y);
            cvSet2D(pIplImage, y, x, CV_RGB(qRed(rgb), qGreen(rgb), qBlue(rgb)));
        }
    }
}

QString __Default_Cfg_Param_String(void)
{
    QString dft_cfg_file = "128 Bin\n";
    dft_cfg_file += "0 Inv\n";
    dft_cfg_file += "0 Otus\n";
    dft_cfg_file += "0 Ero_Round\n";
    dft_cfg_file += "0 Dia_Round\n";
    dft_cfg_file += "50 PinOut(ms)\n";
    dft_cfg_file += "100 Area_Low\n";
    dft_cfg_file += "49999 Area_High\n";
    dft_cfg_file += "0 Channel\n";
    dft_cfg_file += "10\n";
    dft_cfg_file += "10\n";
    dft_cfg_file += "630\n";
    dft_cfg_file += "10\n";
    dft_cfg_file += "630\n";
    dft_cfg_file += "470\n";
    dft_cfg_file += "10\n";
    dft_cfg_file += "470\n";

    return dft_cfg_file;
}

int __Cam_Config_File_Check(unsigned int cam_id)
{
    QString fileName;

    if(cam_id >= CAM_NUM_MAX){
        return ERR_CAM_ID;
    }

    fileName = QDir::currentPath()+gCamCfgFile[cam_id].cfg_file_name;
    QFile file(fileName);

    if(!file.exists()){
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream ts(&file);
        QString cfg_para = __Default_Cfg_Param_String();
        ts << cfg_para << endl;
    }
    file.close();
    return 0;
}

void MainWindow::ShowPorcessedPic(unsigned int cam_id, QImage *qBinImage)
{
    switch(cam_id){
        case 0:
            ScaleImage2Label(*qBinImage, ui->cam0_processed_pic);
            ui->cam0_processed_pic->setPixmap(QPixmap::fromImage(*qBinImage));
            ui->cam0_processed_pic->setAlignment(Qt::AlignCenter);
            break;
        case 1:
            ScaleImage2Label(*qBinImage, ui->cam1_processed_pic);
            ui->cam1_processed_pic->setPixmap(QPixmap::fromImage(*qBinImage));
            ui->cam1_processed_pic->setAlignment(Qt::AlignCenter);
            break;
        case 2:
            ScaleImage2Label(*qBinImage, ui->cam2_processed_pic);
            ui->cam2_processed_pic->setPixmap(QPixmap::fromImage(*qBinImage));
            ui->cam2_processed_pic->setAlignment(Qt::AlignCenter);
            break;
        case 3:
            ScaleImage2Label(*qBinImage, ui->cam3_processed_pic);
            ui->cam3_processed_pic->setPixmap(QPixmap::fromImage(*qBinImage));
            ui->cam3_processed_pic->setAlignment(Qt::AlignCenter);
            break;
    }
}

void MainWindow::ShowSourcePic(unsigned int cam_id, QImage *qColorImage)
{
    switch(cam_id){
        case 0:
            ScaleImage2Label(*qColorImage, ui->cam0_source_pic);
            ui->cam0_source_pic->setPixmap(QPixmap::fromImage(*qColorImage));
            ui->cam0_source_pic->setAlignment(Qt::AlignCenter);
            break;
        case 1:
            ScaleImage2Label(*qColorImage, ui->cam1_source_pic);
            ui->cam1_source_pic->setPixmap(QPixmap::fromImage(*qColorImage));
            ui->cam1_source_pic->setAlignment(Qt::AlignCenter);
            break;
        case 2:
            ScaleImage2Label(*qColorImage, ui->cam2_source_pic);
            ui->cam2_source_pic->setPixmap(QPixmap::fromImage(*qColorImage));
            ui->cam2_source_pic->setAlignment(Qt::AlignCenter);
            break;
        case 3:
            ScaleImage2Label(*qColorImage, ui->cam3_source_pic);
            ui->cam3_source_pic->setPixmap(QPixmap::fromImage(*qColorImage));
            ui->cam3_source_pic->setAlignment(Qt::AlignCenter);
            break;
    }
}

QString MainWindow::SourePicPathGet(unsigned int cam_id)
{
    switch(cam_id){
        case 0:
            return ui->cam0_src_pic_path_line_edit->text();
            break;
        case 1:
            return ui->cam1_src_pic_path_line_edit->text();
            break;
        case 2:
            return ui->cam2_src_pic_path_line_edit->text();
            break;
        case 3:
            return ui->cam3_src_pic_path_line_edit->text();
            break;
    }
    /* if cam_id not match,return the first tab source path */
    return ui->cam0_src_pic_path_line_edit->text();
}

void MainWindow::ResultSet(unsigned int cam_id, const QString *result)
{
    switch(cam_id){
        case 0:
            ui->cam0_result->setPlainText(result->toLatin1().data());
            break;
        case 1:
            ui->cam1_result->setPlainText(result->toLatin1().data());
            break;
        case 2:
            ui->cam2_result->setPlainText(result->toLatin1().data());
            break;
        case 3:
            ui->cam3_result->setPlainText(result->toLatin1().data());
            break;
    }
}

int MainWindow::Pic_Process(unsigned int cam_id)
{
    if(cam_id >= CAM_NUM_MAX){
        return ERR_CAM_ID;
    }

    if(0 == PicLoadFlag[cam_id]){
        return ERR_NO_PIC_LOAD;
    }
    QString result = QString("Camera%1\n").arg(cam_id);
    QString file_name = SourePicPathGet(cam_id);
    int arr[1];
    arr[0] = 4;
    CvPoint ** SelectArea = new CvPoint*[1];
    SelectArea[0] = new CvPoint[4];
    SelectArea[0][0] = cvPoint(cfg_param[cam_id].checkarea[0].x, cfg_param[cam_id].checkarea[0].y);
    SelectArea[0][1] = cvPoint(cfg_param[cam_id].checkarea[1].x, cfg_param[cam_id].checkarea[1].y);
    SelectArea[0][2] = cvPoint(cfg_param[cam_id].checkarea[2].x, cfg_param[cam_id].checkarea[2].y);
    SelectArea[0][3] = cvPoint(cfg_param[cam_id].checkarea[3].x, cfg_param[cam_id].checkarea[3].y);
    IplImage *ColorImg = cvLoadImage(file_name.toLatin1().data());
    IplImage *GrayImage = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 1 );
    IplImage *BinImage = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 1 );
    //IplImage *AdpBinImage_M = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 1 );
    //IplImage *AdpBinImage_G = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 1 );
    IplImage *EroImage = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 1 );
    IplImage *DiaImage = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 1 );
    IplImage *EmptyImage = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 1 );
    cvFillPoly(EmptyImage,SelectArea,arr,1,CV_RGB(255,255,255));
    IplImage * pImageChannel[4] = {0,0,0,0};//分别保存4个通道的灰度图像
    //创建各个灰度图像
    for(int i = 0; i < ColorImg->nChannels; i++)
        pImageChannel[i] = cvCreateImage(cvGetSize(ColorImg),ColorImg->depth,1);

    if(cfg_param[cam_id].channel){
        int channel = cfg_param[cam_id].channel - 1;
        //分割通道
        cvSplit(ColorImg,pImageChannel[0],pImageChannel[1],pImageChannel[2],pImageChannel[3]);
        //对所选信道分别做直方图均衡化
        cvEqualizeHist(pImageChannel[channel],pImageChannel[channel]);
        int threshold_last_flag = (cfg_param[cam_id].invert_flag?CV_THRESH_BINARY_INV:CV_THRESH_BINARY)|(cfg_param[cam_id].otus_flag?CV_THRESH_OTSU:0);
        cvThreshold(pImageChannel[channel], BinImage, cfg_param[cam_id].bin_threshold, 255, threshold_last_flag);
#if 0
        cvAdaptiveThreshold(pImageChannel[channel], AdpBinImage_M, 255, CV_ADAPTIVE_THRESH_MEAN_C, cfg_param[cam_id].invert_flag?CV_THRESH_BINARY_INV:CV_THRESH_BINARY, 9,5);
        cvAdaptiveThreshold(pImageChannel[channel], AdpBinImage_G, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, cfg_param[cam_id].invert_flag?CV_THRESH_BINARY_INV:CV_THRESH_BINARY, 9,5);
        cvNamedWindow("Binary",1);
        cvNamedWindow("AdpBinary_M",1);
        cvNamedWindow("AdpBinary_G",1);
        cvShowImage("Binary", BinImage);
        cvShowImage("AdpBinary_M", AdpBinImage_M);
        cvShowImage("AdpBinary_G", AdpBinImage_G);
#endif
    }else{
        cvCvtColor(ColorImg, GrayImage, CV_BGR2GRAY);
        int threshold_last_flag = (cfg_param[cam_id].invert_flag?CV_THRESH_BINARY_INV:CV_THRESH_BINARY)|(cfg_param[cam_id].otus_flag?CV_THRESH_OTSU:0);
        cvThreshold(GrayImage, BinImage, cfg_param[cam_id].bin_threshold, 255, threshold_last_flag);
    }

    cvAnd(EmptyImage, BinImage, BinImage, NULL);
    cvErode(BinImage, EroImage, NULL, cfg_param[cam_id].ero_round);
    cvDilate(EroImage, DiaImage, NULL, cfg_param[cam_id].dia_round);
    QImage *qBinImage = IplImageToGrayQImage(DiaImage);
    ShowPorcessedPic(cam_id, qBinImage);
    /* 查找连通域，画框框 */
    CvSeq *contour = 0;
    CvMemStorage *storage = cvCreateMemStorage(0);
    cvFindContours(DiaImage, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    int area_size = 0;
    int valid_area_num = 0;
    CvRect area_rect;
    for(;contour!=0;contour=contour->h_next) {
        area_size = cvContourArea(contour, CV_WHOLE_SEQ);
        if(area_size < cfg_param[cam_id].abnormal_area_low || area_size > cfg_param[cam_id].abnormal_area_high)
            continue;
        valid_area_num += 1;
        result += QString("    Area(%1): %2\n").arg(valid_area_num).arg(area_size);
        area_rect = cvBoundingRect(contour);
        cvRectangle(ColorImg, cvPoint(area_rect.x, area_rect.y),
                    cvPoint(area_rect.x+area_rect.width, area_rect.y+area_rect.height),
                    cvScalar(0, 255, 255),2);
    }
    result += QString("Valid Area(s):%1\n").arg(valid_area_num);
    ResultSet(cam_id, &result);
    cvPolyLine(ColorImg, SelectArea,arr,1,1,CV_RGB(250,0,0));
    QImage *qColorImage = IplImageToQImage(ColorImg);
    ShowSourcePic(cam_id, qColorImage);

    cvReleaseMemStorage(&storage);
    cvReleaseImage(&ColorImg);
    cvReleaseImage(&pImageChannel[0]);
    cvReleaseImage(&pImageChannel[1]);
    cvReleaseImage(&pImageChannel[2]);
    cvReleaseImage(&pImageChannel[3]);
    cvReleaseImage(&GrayImage);
    cvReleaseImage(&BinImage);
    //cvReleaseImage(&AdpBinImage_M);
    //cvReleaseImage(&AdpBinImage_G);
    cvReleaseImage(&EroImage);
    cvReleaseImage(&DiaImage);
    cvReleaseImage(&EmptyImage);

    return 0;
}

int MainWindow::Cam_Config_File_Load(unsigned int cam_id)
{
    int data_array[15] = {0};
    QString fileName;
    if(cam_id >= CAM_NUM_MAX){
        return ERR_CAM_ID;
    }

    fileName = QDir::currentPath()+gCamCfgFile[cam_id].cfg_file_name;
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)){
        return ERR_CFG_FILE_NONE;
    }

    int data_index = 0;
    int index_kongge = 0;
    QString kongge = " ";
    while(!file.atEnd()){
        QString lineString=QString(file.readLine());
        index_kongge = lineString.indexOf(kongge);
        if(index_kongge > 0){
            lineString = lineString.left(index_kongge);
        }
        data_array[data_index] = lineString.toInt();
        data_index++;
    }
    cfg_param[cam_id].bin_threshold = data_array[0];
    cfg_param[cam_id].invert_flag = data_array[1];
    cfg_param[cam_id].otus_flag = data_array[2];
    cfg_param[cam_id].ero_round = data_array[3];
    cfg_param[cam_id].dia_round = data_array[4];
    cfg_param[cam_id].pinout_ms = data_array[5];
    cfg_param[cam_id].abnormal_area_low = data_array[6];
    cfg_param[cam_id].abnormal_area_high = data_array[7];
    cfg_param[cam_id].channel = data_array[8];
    cfg_param[cam_id].checkarea[0].x = data_array[9];
    cfg_param[cam_id].checkarea[0].y = data_array[10];
    cfg_param[cam_id].checkarea[1].x = data_array[11];
    cfg_param[cam_id].checkarea[1].y = data_array[12];
    cfg_param[cam_id].checkarea[2].x = data_array[13];
    cfg_param[cam_id].checkarea[2].y = data_array[14];
    cfg_param[cam_id].checkarea[3].x = data_array[15];
    cfg_param[cam_id].checkarea[3].y = data_array[16];
    file.close();
    return 0;
}

void MainWindow::Cam_Param_Init(void)
{
    int ret = 0;
    unsigned int cam_id = 0;

    for(cam_id = 0; cam_id < CAM_NUM_MAX; cam_id++){
        PicLoadFlag[cam_id] = 0;
        AreaSelectActivePoint[cam_id] = 0;
        ret = __Cam_Config_File_Check(cam_id);
        if(0 != ret){
            printf("Camera%d config file not exist\n", cam_id);
        }
        Cam_Config_File_Load(cam_id);
    }
}

void MainWindow::Cam0_Ui_Init(void)
{
    ui->cam0_bin_inv->setChecked(cfg_param[0].invert_flag?true:false);
    ui->cam0_otus_binary->setChecked(cfg_param[0].otus_flag?true:false);
    ui->cam0_bin_threshold_slider->setValue(cfg_param[0].bin_threshold);
    ui->cam0_bin_threshold_value->setText(QString::number(cfg_param[0].bin_threshold));
    ui->cam0_ero_round_slider->setValue(cfg_param[0].ero_round);
    ui->cam0_ero_round_value->setText(QString::number(cfg_param[0].ero_round));
    ui->cam0_dia_round_slider->setValue(cfg_param[0].dia_round);
    ui->cam0_dia_round_value->setText(QString::number(cfg_param[0].dia_round));
    ui->cam0_abnormal_area_high_slider->setValue(cfg_param[0].abnormal_area_high);
    ui->cam0_abnormal_area_high_value->setText(QString::number(cfg_param[0].abnormal_area_high));
    ui->cam0_abnormal_area_low_slider->setValue(cfg_param[0].abnormal_area_low);
    ui->cam0_abnormal_area_low_value->setText(QString::number(cfg_param[0].abnormal_area_low));
    QString cfg_file_path = QDir::currentPath()+gCamCfgFile[0].cfg_file_name;
    ui->cam0_cfg_file_path->setText(cfg_file_path);
    ui->cam0_check_area_point_hslider->setValue(cfg_param[0].checkarea[0].x);
    ui->cam0_check_area_point_vslider->setValue(cfg_param[0].checkarea[0].y);
    ui->cam0_color_channel->setChecked(FALSE);
    ui->cam0_r_channel->setChecked(FALSE);
    ui->cam0_g_channel->setChecked(FALSE);
    ui->cam0_b_channel->setChecked(FALSE);
    switch(cfg_param[0].channel){
        case 0:
            ui->cam0_color_channel->setChecked(TRUE);
            break;
        case 1:
            ui->cam0_r_channel->setChecked(TRUE);
            break;
        case 2:
            ui->cam0_g_channel->setChecked(TRUE);
            break;
        case 3:
            ui->cam0_b_channel->setChecked(TRUE);
            break;
    }
    ui->cam0_bin_threshold_slider->setEnabled(cfg_param[0].otus_flag?FALSE:TRUE);
    ui->cam0_bin_th_dec->setEnabled(cfg_param[0].otus_flag?FALSE:TRUE);
    ui->cam0_bin_th_inc->setEnabled(cfg_param[0].otus_flag?FALSE:TRUE);
    ui->cam0_bin_threshold_value->setEnabled(cfg_param[0].otus_flag?FALSE:TRUE);
}

void MainWindow::Cam1_Ui_Init(void)
{
    ui->cam1_bin_inv->setChecked(cfg_param[1].invert_flag?true:false);
    ui->cam1_otus_binary->setChecked(cfg_param[1].otus_flag?true:false);
    ui->cam1_bin_threshold_slider->setValue(cfg_param[1].bin_threshold);
    ui->cam1_bin_threshold_value->setText(QString::number(cfg_param[1].bin_threshold));
    ui->cam1_ero_round_slider->setValue(cfg_param[1].ero_round);
    ui->cam1_ero_round_value->setText(QString::number(cfg_param[1].ero_round));
    ui->cam1_dia_round_slider->setValue(cfg_param[1].dia_round);
    ui->cam1_dia_round_value->setText(QString::number(cfg_param[1].dia_round));
    ui->cam1_abnormal_area_high_slider->setValue(cfg_param[1].abnormal_area_high);
    ui->cam1_abnormal_area_high_value->setText(QString::number(cfg_param[1].abnormal_area_high));
    ui->cam1_abnormal_area_low_slider->setValue(cfg_param[1].abnormal_area_low);
    ui->cam1_abnormal_area_low_value->setText(QString::number(cfg_param[1].abnormal_area_low));
    QString cfg_file_path = QDir::currentPath()+gCamCfgFile[1].cfg_file_name;
    ui->cam1_cfg_file_path->setText(cfg_file_path);
    ui->cam1_check_area_point_hslider->setValue(cfg_param[1].checkarea[0].x);
    ui->cam1_check_area_point_vslider->setValue(cfg_param[1].checkarea[0].y);
    ui->cam1_color_channel->setChecked(FALSE);
    ui->cam1_r_channel->setChecked(FALSE);
    ui->cam1_g_channel->setChecked(FALSE);
    ui->cam1_b_channel->setChecked(FALSE);
    switch(cfg_param[1].channel){
        case 0:
            ui->cam1_color_channel->setChecked(TRUE);
            break;
        case 1:
            ui->cam1_r_channel->setChecked(TRUE);
            break;
        case 2:
            ui->cam1_g_channel->setChecked(TRUE);
            break;
        case 3:
            ui->cam1_b_channel->setChecked(TRUE);
            break;
    }
    ui->cam1_bin_threshold_slider->setEnabled(cfg_param[1].otus_flag?FALSE:TRUE);
    ui->cam1_bin_th_dec->setEnabled(cfg_param[1].otus_flag?FALSE:TRUE);
    ui->cam1_bin_th_inc->setEnabled(cfg_param[1].otus_flag?FALSE:TRUE);
    ui->cam1_bin_threshold_value->setEnabled(cfg_param[1].otus_flag?FALSE:TRUE);
}

void MainWindow::Cam2_Ui_Init(void)
{
    ui->cam2_bin_inv->setChecked(cfg_param[2].invert_flag?true:false);
    ui->cam2_otus_binary->setChecked(cfg_param[2].otus_flag?true:false);
    ui->cam2_bin_threshold_slider->setValue(cfg_param[2].bin_threshold);
    ui->cam2_bin_threshold_value->setText(QString::number(cfg_param[2].bin_threshold));
    ui->cam2_ero_round_slider->setValue(cfg_param[2].ero_round);
    ui->cam2_ero_round_value->setText(QString::number(cfg_param[2].ero_round));
    ui->cam2_dia_round_slider->setValue(cfg_param[2].dia_round);
    ui->cam2_dia_round_value->setText(QString::number(cfg_param[2].dia_round));
    ui->cam2_abnormal_area_high_slider->setValue(cfg_param[2].abnormal_area_high);
    ui->cam2_abnormal_area_high_value->setText(QString::number(cfg_param[2].abnormal_area_high));
    ui->cam2_abnormal_area_low_slider->setValue(cfg_param[2].abnormal_area_low);
    ui->cam2_abnormal_area_low_value->setText(QString::number(cfg_param[2].abnormal_area_low));
    QString cfg_file_path = QDir::currentPath()+gCamCfgFile[2].cfg_file_name;
    ui->cam2_cfg_file_path->setText(cfg_file_path);
    ui->cam2_check_area_point_hslider->setValue(cfg_param[2].checkarea[0].x);
    ui->cam2_check_area_point_vslider->setValue(cfg_param[2].checkarea[0].y);
    ui->cam2_color_channel->setChecked(FALSE);
    ui->cam2_r_channel->setChecked(FALSE);
    ui->cam2_g_channel->setChecked(FALSE);
    ui->cam2_b_channel->setChecked(FALSE);
    switch(cfg_param[2].channel){
        case 0:
            ui->cam2_color_channel->setChecked(TRUE);
            break;
        case 1:
            ui->cam2_r_channel->setChecked(TRUE);
            break;
        case 2:
            ui->cam2_g_channel->setChecked(TRUE);
            break;
        case 3:
            ui->cam2_b_channel->setChecked(TRUE);
            break;
    }
    ui->cam2_bin_threshold_slider->setEnabled(cfg_param[2].otus_flag?FALSE:TRUE);
    ui->cam2_bin_th_dec->setEnabled(cfg_param[2].otus_flag?FALSE:TRUE);
    ui->cam2_bin_th_inc->setEnabled(cfg_param[2].otus_flag?FALSE:TRUE);
    ui->cam2_bin_threshold_value->setEnabled(cfg_param[2].otus_flag?FALSE:TRUE);
}

void MainWindow::Cam3_Ui_Init(void)
{
    ui->cam3_bin_inv->setChecked(cfg_param[3].invert_flag?true:false);
    ui->cam3_otus_binary->setChecked(cfg_param[3].otus_flag?true:false);
    ui->cam3_bin_threshold_slider->setValue(cfg_param[3].bin_threshold);
    ui->cam3_bin_threshold_value->setText(QString::number(cfg_param[3].bin_threshold));
    ui->cam3_ero_round_slider->setValue(cfg_param[3].ero_round);
    ui->cam3_ero_round_value->setText(QString::number(cfg_param[3].ero_round));
    ui->cam3_dia_round_slider->setValue(cfg_param[3].dia_round);
    ui->cam3_dia_round_value->setText(QString::number(cfg_param[3].dia_round));
    ui->cam3_abnormal_area_high_slider->setValue(cfg_param[3].abnormal_area_high);
    ui->cam3_abnormal_area_high_value->setText(QString::number(cfg_param[3].abnormal_area_high));
    ui->cam3_abnormal_area_low_slider->setValue(cfg_param[3].abnormal_area_low);
    ui->cam3_abnormal_area_low_value->setText(QString::number(cfg_param[3].abnormal_area_low));
    QString cfg_file_path = QDir::currentPath()+gCamCfgFile[3].cfg_file_name;
    ui->cam3_cfg_file_path->setText(cfg_file_path);
    ui->cam3_check_area_point_hslider->setValue(cfg_param[3].checkarea[0].x);
    ui->cam3_check_area_point_vslider->setValue(cfg_param[3].checkarea[0].y);
    ui->cam3_color_channel->setChecked(FALSE);
    ui->cam3_r_channel->setChecked(FALSE);
    ui->cam3_g_channel->setChecked(FALSE);
    ui->cam3_b_channel->setChecked(FALSE);
    switch(cfg_param[3].channel){
        case 0:
            ui->cam3_color_channel->setChecked(TRUE);
            break;
        case 1:
            ui->cam3_r_channel->setChecked(TRUE);
            break;
        case 2:
            ui->cam3_g_channel->setChecked(TRUE);
            break;
        case 3:
            ui->cam3_b_channel->setChecked(TRUE);
            break;
    }
    ui->cam3_bin_threshold_slider->setEnabled(cfg_param[3].otus_flag?FALSE:TRUE);
    ui->cam3_bin_th_dec->setEnabled(cfg_param[3].otus_flag?FALSE:TRUE);
    ui->cam3_bin_th_inc->setEnabled(cfg_param[3].otus_flag?FALSE:TRUE);
    ui->cam3_bin_threshold_value->setEnabled(cfg_param[3].otus_flag?FALSE:TRUE);
}

void MainWindow::handleResults(const QString &s)
{

}

void MainWindow::Data_init(void)
{
    NetworkThread *networkThread = new NetworkThread;
    Cam_Param_Init();
    Cam0_Ui_Init();
    Cam1_Ui_Init();
    Cam2_Ui_Init();
    Cam3_Ui_Init();
    ui->cam_gpio_output_pluse_width->setValue(cfg_param[0].pinout_ms);
    ui->cam_gpio_pluse_width->setText(QString::number(cfg_param[0].pinout_ms)+"ms");
    connect(networkThread, &NetworkThread::resultReady, this, &MainWindow::handleResults);
    connect(networkThread, &NetworkThread::finished, networkThread, &QObject::deleteLater);
    networkThread->start();
}

/* 相机0的相关函数 */
void MainWindow::on_cam0_bin_inv_clicked()
{
    cfg_param[0].invert_flag = ui->cam0_bin_inv->isChecked()?1:0;
    MainWindow::Pic_Process(0);
}

void MainWindow::on_cam0_bin_th_dec_clicked()
{
    cfg_param[0].bin_threshold = cfg_param[0].bin_threshold > 0?(cfg_param[0].bin_threshold - 1):0;
    ui->cam0_bin_threshold_slider->setValue(cfg_param[0].bin_threshold);
    ui->cam0_bin_threshold_value->setText(QString::number(cfg_param[0].bin_threshold));
    MainWindow::Pic_Process(0);
}

void MainWindow::on_cam0_bin_th_inc_clicked()
{
    cfg_param[0].bin_threshold = cfg_param[0].bin_threshold < 255?(cfg_param[0].bin_threshold + 1):255;
    ui->cam0_bin_threshold_slider->setValue(cfg_param[0].bin_threshold);
    ui->cam0_bin_threshold_value->setText(QString::number(cfg_param[0].bin_threshold));
    MainWindow::Pic_Process(0);
}

void MainWindow::on_cam0_bin_threshold_slider_valueChanged(int value)
{
    cfg_param[0].bin_threshold = value;
    ui->cam0_bin_threshold_value->setText(QString::number(cfg_param[0].bin_threshold));
    MainWindow::Pic_Process(0);
}

void MainWindow::on_cam0_ero_rnd_dec_clicked()
{
    cfg_param[0].ero_round = cfg_param[0].ero_round > 0?(cfg_param[0].ero_round - 1):0;
    ui->cam0_ero_round_slider->setValue(cfg_param[0].ero_round);
    ui->cam0_ero_round_value->setText(QString::number(cfg_param[0].ero_round));
    MainWindow::Pic_Process(0);
}

void MainWindow::on_cam0_ero_rnd_inc_clicked()
{
    cfg_param[0].ero_round = cfg_param[0].ero_round < 10?(cfg_param[0].ero_round + 1):10;
    ui->cam0_ero_round_slider->setValue(cfg_param[0].ero_round);
    ui->cam0_ero_round_value->setText(QString::number(cfg_param[0].ero_round));
    MainWindow::Pic_Process(0);
}

void MainWindow::on_cam0_ero_round_slider_valueChanged(int value)
{
    cfg_param[0].ero_round = value;
    ui->cam0_ero_round_value->setText(QString::number(cfg_param[0].ero_round));
    MainWindow::Pic_Process(0);
}

void MainWindow::on_cam0_dia_rnd_dec_clicked()
{
    cfg_param[0].dia_round = cfg_param[0].dia_round > 0?(cfg_param[0].dia_round - 1):0;
    ui->cam0_dia_round_slider->setValue(cfg_param[0].dia_round);
    ui->cam0_dia_round_value->setText(QString::number(cfg_param[0].dia_round));
    MainWindow::Pic_Process(0);
}

void MainWindow::on_cam0_dia_rnd_inc_clicked()
{
    cfg_param[0].dia_round = cfg_param[0].dia_round < 10?(cfg_param[0].dia_round + 1):10;
    ui->cam0_dia_round_slider->setValue(cfg_param[0].dia_round);
    ui->cam0_dia_round_value->setText(QString::number(cfg_param[0].dia_round));
    MainWindow::Pic_Process(0);
}

void MainWindow::on_cam0_dia_round_slider_valueChanged(int value)
{
    cfg_param[0].dia_round = value;
    ui->cam0_dia_round_value->setText(QString::number(cfg_param[0].dia_round));
    MainWindow::Pic_Process(0);
}

void MainWindow::on_cam0_ab_area_high_dec_clicked()
{
    int crt_value = cfg_param[0].abnormal_area_high;
    if(crt_value > 0 && crt_value > cfg_param[0].abnormal_area_low){
        cfg_param[0].abnormal_area_high -= 1;
    }else{
        cfg_param[0].abnormal_area_high = 0;
    }
    ui->cam0_abnormal_area_high_slider->setValue(cfg_param[0].abnormal_area_high);
    ui->cam0_abnormal_area_high_value->setText(QString::number(cfg_param[0].abnormal_area_high));
    MainWindow::Pic_Process(0);
}

void MainWindow::on_cam0_ab_area_high_inc_clicked()
{
    int crt_value = cfg_param[0].abnormal_area_high;
    if(crt_value < 300000 && crt_value > cfg_param[0].abnormal_area_low){
        cfg_param[0].abnormal_area_high += 1;
    }else{
        cfg_param[0].abnormal_area_high = 300000;
    }
    ui->cam0_abnormal_area_high_slider->setValue(cfg_param[0].abnormal_area_high);
    ui->cam0_abnormal_area_high_value->setText(QString::number(cfg_param[0].abnormal_area_high));
    MainWindow::Pic_Process(0);
}

void MainWindow::on_cam0_abnormal_area_high_slider_valueChanged(int value)
{
    if(value < cfg_param[0].abnormal_area_low){
        int modify_value = (cfg_param[0].abnormal_area_low+1 < 300000)?(cfg_param[0].abnormal_area_low+1):300000;
        cfg_param[0].abnormal_area_high = modify_value;
        ui->cam0_abnormal_area_high_value->setText(QString::number(cfg_param[0].abnormal_area_high));
        ui->cam0_abnormal_area_high_slider->setValue(cfg_param[0].abnormal_area_high);
    }else{
        cfg_param[0].abnormal_area_high = value;
        ui->cam0_abnormal_area_high_value->setText(QString::number(cfg_param[0].abnormal_area_high));
    }
    MainWindow::Pic_Process(0);
}

void MainWindow::on_cam0_ab_area_low_dec_clicked()
{
    int crt_value = cfg_param[0].abnormal_area_low;
    if(crt_value > 0 && crt_value < cfg_param[0].abnormal_area_high){
        cfg_param[0].abnormal_area_low -= 1;
    }else{
        cfg_param[0].abnormal_area_low = 0;
    }
    ui->cam0_abnormal_area_low_slider->setValue(cfg_param[0].abnormal_area_low);
    ui->cam0_abnormal_area_low_value->setText(QString::number(cfg_param[0].abnormal_area_low));
    MainWindow::Pic_Process(0);
}

void MainWindow::on_cam0_ab_area_low_inc_clicked()
{
    int crt_value = cfg_param[0].abnormal_area_low;
    if(crt_value < 300000 && crt_value < cfg_param[0].abnormal_area_high){
        cfg_param[0].abnormal_area_low += 1;
    }else{
        cfg_param[0].abnormal_area_low = 300000;
    }
    ui->cam0_abnormal_area_low_slider->setValue(cfg_param[0].abnormal_area_low);
    ui->cam0_abnormal_area_low_value->setText(QString::number(cfg_param[0].abnormal_area_low));
    MainWindow::Pic_Process(0);
}

void MainWindow::on_cam0_abnormal_area_low_slider_valueChanged(int value)
{
    if(value > cfg_param[0].abnormal_area_high){
        int modify_value = (cfg_param[0].abnormal_area_high > 1)?(cfg_param[0].abnormal_area_high-1):0;
        cfg_param[0].abnormal_area_low = modify_value;
        ui->cam0_abnormal_area_low_value->setText(QString::number(cfg_param[0].abnormal_area_low));
        ui->cam0_abnormal_area_low_slider->setValue(cfg_param[0].abnormal_area_low);
    }else{
        cfg_param[0].abnormal_area_low = value;
        ui->cam0_abnormal_area_low_value->setText(QString::number(cfg_param[0].abnormal_area_low));
    }
    MainWindow::Pic_Process(0);
}

void MainWindow::on_cam0_src_pic_open_bt_clicked()
{
    QString pic_path = QFileDialog::getOpenFileName(NULL, "选择640x480大小的图片", ".", "image(*.jpeg *.jpg *.png *.bmp)");
    /* 判断是否点了取消操作导致路径为空 */
    if(!QString(pic_path).isEmpty()){
        ui->cam0_src_pic_path_line_edit->setText(pic_path);
        QImage *ori_Image = new QImage(pic_path);
        ScaleImage[0] = ScaleImage2Label(*ori_Image, ui->cam0_source_pic);
        ui->cam0_source_pic->setPixmap(QPixmap::fromImage(ScaleImage[0]));
        ui->cam0_source_pic->setAlignment(Qt::AlignCenter);
        PicLoadFlag[0] = 1;
        Pic_Process(0);
    }
}

void MainWindow::on_cam0_cfg_file_reload_clicked()
{
    Cam_Config_File_Load(0);
    Cam0_Ui_Init();
    MainWindow::Pic_Process(0);
}

void MainWindow::on_cam0_cfg_local_save_clicked()
{
    CFG_PARAM *pCfgParam = &cfg_param[0];
    QString crt_cfg_param = QString("%1 Bin\n%2 Inv\n%3 Otus\n").arg(pCfgParam->bin_threshold).arg(pCfgParam->invert_flag).arg(pCfgParam->otus_flag);
    crt_cfg_param += QString("%1 Ero_Round\n%2 Dia_Round\n").arg(pCfgParam->ero_round).arg(pCfgParam->dia_round);
    crt_cfg_param += QString("%1 PinOut(ms)\n").arg(pCfgParam->pinout_ms);
    crt_cfg_param += QString("%1 Area_Low\n%2 Area_High\n").arg(pCfgParam->abnormal_area_low).arg(pCfgParam->abnormal_area_high);
    crt_cfg_param += QString("%1\n%2\n").arg(pCfgParam->checkarea[0].x).arg(pCfgParam->checkarea[0].y);
    crt_cfg_param += QString("%1\n%2\n").arg(pCfgParam->checkarea[1].x).arg(pCfgParam->checkarea[1].y);
    crt_cfg_param += QString("%1\n%2\n").arg(pCfgParam->checkarea[2].x).arg(pCfgParam->checkarea[2].y);
    crt_cfg_param += QString("%1\n%2\n").arg(pCfgParam->checkarea[3].x).arg(pCfgParam->checkarea[3].y);
    QString fileName = QDir::currentPath()+gCamCfgFile[0].cfg_file_name;
    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream ts(&file);
    ts << crt_cfg_param << endl;
    file.close();
    QMessageBox::about(NULL, "提示", "相机1配置文件保存成功");
}

/* 相机1的相关函数 */
void MainWindow::on_cam1_bin_inv_clicked()
{
    cfg_param[1].invert_flag = ui->cam1_bin_inv->isChecked()?1:0;
    MainWindow::Pic_Process(1);
}

void MainWindow::on_cam1_bin_th_dec_clicked()
{
    cfg_param[1].bin_threshold = cfg_param[1].bin_threshold > 0?(cfg_param[1].bin_threshold - 1):0;
    ui->cam1_bin_threshold_slider->setValue(cfg_param[1].bin_threshold);
    ui->cam1_bin_threshold_value->setText(QString::number(cfg_param[1].bin_threshold));
    MainWindow::Pic_Process(1);
}

void MainWindow::on_cam1_bin_th_inc_clicked()
{
    cfg_param[1].bin_threshold = cfg_param[1].bin_threshold < 255?(cfg_param[1].bin_threshold + 1):255;
    ui->cam1_bin_threshold_slider->setValue(cfg_param[1].bin_threshold);
    ui->cam1_bin_threshold_value->setText(QString::number(cfg_param[1].bin_threshold));
    MainWindow::Pic_Process(1);
}

void MainWindow::on_cam1_bin_threshold_slider_valueChanged(int value)
{
    cfg_param[1].bin_threshold = value;
    ui->cam1_bin_threshold_value->setText(QString::number(cfg_param[1].bin_threshold));
    MainWindow::Pic_Process(1);
}

void MainWindow::on_cam1_ero_rnd_dec_clicked()
{
    cfg_param[1].ero_round = cfg_param[1].ero_round > 0?(cfg_param[1].ero_round - 1):0;
    ui->cam1_ero_round_slider->setValue(cfg_param[1].ero_round);
    ui->cam1_ero_round_value->setText(QString::number(cfg_param[1].ero_round));
    MainWindow::Pic_Process(1);
}

void MainWindow::on_cam1_ero_rnd_inc_clicked()
{
    cfg_param[1].ero_round = cfg_param[1].ero_round < 10?(cfg_param[1].ero_round + 1):10;
    ui->cam1_ero_round_slider->setValue(cfg_param[1].ero_round);
    ui->cam1_ero_round_value->setText(QString::number(cfg_param[1].ero_round));
    MainWindow::Pic_Process(1);
}

void MainWindow::on_cam1_ero_round_slider_valueChanged(int value)
{
    cfg_param[1].ero_round = value;
    ui->cam1_ero_round_value->setText(QString::number(cfg_param[1].ero_round));
    MainWindow::Pic_Process(1);
}

void MainWindow::on_cam1_dia_rnd_dec_clicked()
{
    cfg_param[1].dia_round = cfg_param[1].dia_round > 0?(cfg_param[1].dia_round - 1):0;
    ui->cam1_dia_round_slider->setValue(cfg_param[1].dia_round);
    ui->cam1_dia_round_value->setText(QString::number(cfg_param[1].dia_round));
    MainWindow::Pic_Process(1);
}

void MainWindow::on_cam1_dia_rnd_inc_clicked()
{
    cfg_param[1].dia_round = cfg_param[1].dia_round < 10?(cfg_param[1].dia_round + 1):10;
    ui->cam1_dia_round_slider->setValue(cfg_param[1].dia_round);
    ui->cam1_dia_round_value->setText(QString::number(cfg_param[1].dia_round));
    MainWindow::Pic_Process(1);
}

void MainWindow::on_cam1_dia_round_slider_valueChanged(int value)
{
    cfg_param[1].dia_round = value;
    ui->cam1_dia_round_value->setText(QString::number(cfg_param[1].dia_round));
    MainWindow::Pic_Process(1);
}

void MainWindow::on_cam1_ab_area_high_dec_clicked()
{
    int crt_value = cfg_param[1].abnormal_area_high;
    if(crt_value > 0 && crt_value > cfg_param[1].abnormal_area_low){
        cfg_param[1].abnormal_area_high -= 1;
    }else{
        cfg_param[1].abnormal_area_high = 0;
    }
    ui->cam1_abnormal_area_high_slider->setValue(cfg_param[1].abnormal_area_high);
    ui->cam1_abnormal_area_high_value->setText(QString::number(cfg_param[1].abnormal_area_high));
    MainWindow::Pic_Process(1);
}

void MainWindow::on_cam1_ab_area_high_inc_clicked()
{
    int crt_value = cfg_param[1].abnormal_area_high;
    if(crt_value < 300000 && crt_value > cfg_param[1].abnormal_area_low){
        cfg_param[1].abnormal_area_high += 1;
    }else{
        cfg_param[1].abnormal_area_high = 300000;
    }
    ui->cam1_abnormal_area_high_slider->setValue(cfg_param[1].abnormal_area_high);
    ui->cam1_abnormal_area_high_value->setText(QString::number(cfg_param[1].abnormal_area_high));
    MainWindow::Pic_Process(1);
}

void MainWindow::on_cam1_abnormal_area_high_slider_valueChanged(int value)
{
    if(value < cfg_param[1].abnormal_area_low){
        int modify_value = (cfg_param[1].abnormal_area_low+1 < 300000)?(cfg_param[1].abnormal_area_low+1):300000;
        cfg_param[1].abnormal_area_high = modify_value;
        ui->cam1_abnormal_area_high_value->setText(QString::number(cfg_param[1].abnormal_area_high));
        ui->cam1_abnormal_area_high_slider->setValue(cfg_param[1].abnormal_area_high);
    }else{
        cfg_param[1].abnormal_area_high = value;
        ui->cam1_abnormal_area_high_value->setText(QString::number(cfg_param[1].abnormal_area_high));
    }
    MainWindow::Pic_Process(1);
}

void MainWindow::on_cam1_ab_area_low_dec_clicked()
{
    int crt_value = cfg_param[1].abnormal_area_low;
    if(crt_value > 0 && crt_value < cfg_param[1].abnormal_area_high){
        cfg_param[1].abnormal_area_low -= 1;
    }else{
        cfg_param[1].abnormal_area_low = 0;
    }
    ui->cam1_abnormal_area_low_slider->setValue(cfg_param[1].abnormal_area_low);
    ui->cam1_abnormal_area_low_value->setText(QString::number(cfg_param[1].abnormal_area_low));
    MainWindow::Pic_Process(1);
}

void MainWindow::on_cam1_ab_area_low_inc_clicked()
{
    int crt_value = cfg_param[1].abnormal_area_low;
    if(crt_value < 300000 && crt_value < cfg_param[1].abnormal_area_high){
        cfg_param[1].abnormal_area_low += 1;
    }else{
        cfg_param[1].abnormal_area_low = 300000;
    }
    ui->cam1_abnormal_area_low_slider->setValue(cfg_param[1].abnormal_area_low);
    ui->cam1_abnormal_area_low_value->setText(QString::number(cfg_param[1].abnormal_area_low));
    MainWindow::Pic_Process(1);
}

void MainWindow::on_cam1_abnormal_area_low_slider_valueChanged(int value)
{
    if(value > cfg_param[1].abnormal_area_high){
        int modify_value = (cfg_param[1].abnormal_area_high > 1)?(cfg_param[1].abnormal_area_high-1):0;
        cfg_param[1].abnormal_area_low = modify_value;
        ui->cam1_abnormal_area_low_value->setText(QString::number(cfg_param[1].abnormal_area_low));
        ui->cam1_abnormal_area_low_slider->setValue(cfg_param[1].abnormal_area_low);
    }else{
        cfg_param[1].abnormal_area_low = value;
        ui->cam1_abnormal_area_low_value->setText(QString::number(cfg_param[1].abnormal_area_low));
    }
    MainWindow::Pic_Process(1);
}

void MainWindow::on_cam1_src_pic_open_bt_clicked()
{
    QString pic_path = QFileDialog::getOpenFileName(NULL, "选择640x480大小的图片", ".", "image(*.jpeg *.jpg *.png *.bmp)");
    /* 判断是否点了取消操作导致路径为空 */
    if(!QString(pic_path).isEmpty()){
        ui->cam1_src_pic_path_line_edit->setText(pic_path);
        QImage *ori_Image = new QImage(pic_path);
        ScaleImage[1] = ScaleImage2Label(*ori_Image, ui->cam1_source_pic);
        ui->cam1_source_pic->setPixmap(QPixmap::fromImage(ScaleImage[1]));
        ui->cam1_source_pic->setAlignment(Qt::AlignCenter);
        PicLoadFlag[1] = 1;
        MainWindow::Pic_Process(1);
    }
}

void MainWindow::on_cam1_cfg_file_reload_clicked()
{
    Cam_Config_File_Load(1);
    Cam1_Ui_Init();
    MainWindow::Pic_Process(1);
}

void MainWindow::on_cam1_cfg_local_save_clicked()
{
    CFG_PARAM *pCfgParam = &cfg_param[1];
    QString crt_cfg_param = QString("%1 Bin\n%2 Inv\n%3 Otus\n").arg(pCfgParam->bin_threshold).arg(pCfgParam->invert_flag).arg(pCfgParam->otus_flag);
    crt_cfg_param += QString("%1 Ero_Round\n%2 Dia_Round\n").arg(pCfgParam->ero_round).arg(pCfgParam->dia_round);
    crt_cfg_param += QString("%1 PinOut(ms)\n").arg(pCfgParam->pinout_ms);
    crt_cfg_param += QString("%1 Area_Low\n%2 Area_High\n").arg(pCfgParam->abnormal_area_low).arg(pCfgParam->abnormal_area_high);
    crt_cfg_param += QString("%1\n%2\n").arg(pCfgParam->checkarea[0].x).arg(pCfgParam->checkarea[0].y);
    crt_cfg_param += QString("%1\n%2\n").arg(pCfgParam->checkarea[1].x).arg(pCfgParam->checkarea[1].y);
    crt_cfg_param += QString("%1\n%2\n").arg(pCfgParam->checkarea[2].x).arg(pCfgParam->checkarea[2].y);
    crt_cfg_param += QString("%1\n%2\n").arg(pCfgParam->checkarea[3].x).arg(pCfgParam->checkarea[3].y);
    QString fileName = QDir::currentPath()+gCamCfgFile[1].cfg_file_name;
    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream ts(&file);
    ts << crt_cfg_param << endl;
    file.close();
    QMessageBox::about(NULL, "提示", "相机2配置文件保存成功");
}

/* 相机2的相关函数 */
void MainWindow::on_cam2_bin_inv_clicked()
{
    cfg_param[2].invert_flag = ui->cam2_bin_inv->isChecked()?1:0;
    MainWindow::Pic_Process(2);
}

void MainWindow::on_cam2_bin_th_dec_clicked()
{
    cfg_param[2].bin_threshold = cfg_param[2].bin_threshold>0?(cfg_param[2].bin_threshold - 1):0;
    ui->cam2_bin_threshold_slider->setValue(cfg_param[2].bin_threshold);
    ui->cam2_bin_threshold_value->setText(QString::number(cfg_param[2].bin_threshold));
    MainWindow::Pic_Process(2);
}

void MainWindow::on_cam2_bin_th_inc_clicked()
{
    cfg_param[2].bin_threshold = cfg_param[2].bin_threshold<255?(cfg_param[2].bin_threshold + 1):255;
    ui->cam2_bin_threshold_slider->setValue(cfg_param[2].bin_threshold);
    ui->cam2_bin_threshold_value->setText(QString::number(cfg_param[2].bin_threshold));
    MainWindow::Pic_Process(2);
}

void MainWindow::on_cam2_bin_threshold_slider_valueChanged(int value)
{
    cfg_param[2].bin_threshold = value;
    ui->cam2_bin_threshold_value->setText(QString::number(cfg_param[2].bin_threshold));
    MainWindow::Pic_Process(2);
}

void MainWindow::on_cam2_ero_rnd_dec_clicked()
{
    cfg_param[2].ero_round = cfg_param[2].ero_round>0?(cfg_param[2].ero_round - 1):0;
    ui->cam2_ero_round_slider->setValue(cfg_param[2].ero_round);
    ui->cam2_ero_round_value->setText(QString::number(cfg_param[2].ero_round));
    MainWindow::Pic_Process(2);
}

void MainWindow::on_cam2_ero_rnd_inc_clicked()
{
    cfg_param[2].ero_round = cfg_param[2].ero_round<10?(cfg_param[2].ero_round + 1):10;
    ui->cam2_ero_round_slider->setValue(cfg_param[2].ero_round);
    ui->cam2_ero_round_value->setText(QString::number(cfg_param[2].ero_round));
    MainWindow::Pic_Process(2);
}

void MainWindow::on_cam2_ero_round_slider_valueChanged(int value)
{
    cfg_param[2].ero_round = value;
    ui->cam2_ero_round_value->setText(QString::number(cfg_param[2].ero_round));
    MainWindow::Pic_Process(2);
}

void MainWindow::on_cam2_dia_rnd_dec_clicked()
{
    cfg_param[2].dia_round = cfg_param[2].dia_round>0?(cfg_param[2].dia_round - 1):0;
    ui->cam2_dia_round_slider->setValue(cfg_param[2].dia_round);
    ui->cam2_dia_round_value->setText(QString::number(cfg_param[2].dia_round));
    MainWindow::Pic_Process(2);
}

void MainWindow::on_cam2_dia_rnd_inc_clicked()
{
    cfg_param[2].dia_round = cfg_param[2].dia_round<10?(cfg_param[2].dia_round + 1):10;
    ui->cam2_dia_round_slider->setValue(cfg_param[2].dia_round);
    ui->cam2_dia_round_value->setText(QString::number(cfg_param[2].dia_round));
    MainWindow::Pic_Process(2);
}

void MainWindow::on_cam2_dia_round_slider_valueChanged(int value)
{
    cfg_param[2].dia_round = value;
    ui->cam2_dia_round_value->setText(QString::number(cfg_param[2].dia_round));
    MainWindow::Pic_Process(2);
}

void MainWindow::on_cam2_ab_area_high_dec_clicked()
{
    int crt_value = cfg_param[2].abnormal_area_high;
    if(crt_value > 0 && crt_value > cfg_param[2].abnormal_area_low){
        cfg_param[2].abnormal_area_high -= 1;
    }else{
        cfg_param[2].abnormal_area_high = 0;
    }
    ui->cam2_abnormal_area_high_slider->setValue(cfg_param[2].abnormal_area_high);
    ui->cam2_abnormal_area_high_value->setText(QString::number(cfg_param[2].abnormal_area_high));
    MainWindow::Pic_Process(2);
}

void MainWindow::on_cam2_ab_area_high_inc_clicked()
{
    int crt_value = cfg_param[2].abnormal_area_high;
    if(crt_value < 300000 && crt_value > cfg_param[2].abnormal_area_low){
        cfg_param[2].abnormal_area_high += 1;
    }else{
        cfg_param[2].abnormal_area_high = 300000;
    }
    ui->cam2_abnormal_area_high_slider->setValue(cfg_param[2].abnormal_area_high);
    ui->cam2_abnormal_area_high_value->setText(QString::number(cfg_param[2].abnormal_area_high));
    MainWindow::Pic_Process(2);
}

void MainWindow::on_cam2_abnormal_area_high_slider_valueChanged(int value)
{
    if(value < cfg_param[2].abnormal_area_low){
        int modify_value = (cfg_param[2].abnormal_area_low+1 < 300000)?(cfg_param[2].abnormal_area_low+1):300000;
        cfg_param[2].abnormal_area_high = modify_value;
        ui->cam2_abnormal_area_high_value->setText(QString::number(cfg_param[2].abnormal_area_high));
        ui->cam2_abnormal_area_high_slider->setValue(cfg_param[2].abnormal_area_high);
    }else{
        cfg_param[2].abnormal_area_high = value;
        ui->cam2_abnormal_area_high_value->setText(QString::number(cfg_param[2].abnormal_area_high));
    }
    MainWindow::Pic_Process(2);
}

void MainWindow::on_cam2_ab_area_low_dec_clicked()
{
    int crt_value = cfg_param[2].abnormal_area_low;
    if(crt_value > 0 && crt_value < cfg_param[2].abnormal_area_high){
        cfg_param[2].abnormal_area_low -= 1;
    }else{
        cfg_param[2].abnormal_area_low = 0;
    }
    ui->cam2_abnormal_area_low_slider->setValue(cfg_param[2].abnormal_area_low);
    ui->cam2_abnormal_area_low_value->setText(QString::number(cfg_param[2].abnormal_area_low));
    MainWindow::Pic_Process(2);
}

void MainWindow::on_cam2_ab_area_low_inc_clicked()
{
    int crt_value = cfg_param[2].abnormal_area_low;
    if(crt_value < 300000 && crt_value < cfg_param[2].abnormal_area_high){
        cfg_param[2].abnormal_area_low += 1;
    }else{
        cfg_param[2].abnormal_area_low = 300000;
    }
    ui->cam2_abnormal_area_low_slider->setValue(cfg_param[2].abnormal_area_low);
    ui->cam2_abnormal_area_low_value->setText(QString::number(cfg_param[2].abnormal_area_low));
    MainWindow::Pic_Process(2);
}

void MainWindow::on_cam2_abnormal_area_low_slider_valueChanged(int value)
{
    if(value > cfg_param[2].abnormal_area_high){
        int modify_value = (cfg_param[2].abnormal_area_high > 1)?(cfg_param[2].abnormal_area_high-1):0;
        cfg_param[2].abnormal_area_low = modify_value;
        ui->cam2_abnormal_area_low_value->setText(QString::number(cfg_param[2].abnormal_area_low));
        ui->cam2_abnormal_area_low_slider->setValue(cfg_param[2].abnormal_area_low);
    }else{
        cfg_param[2].abnormal_area_low = value;
        ui->cam2_abnormal_area_low_value->setText(QString::number(cfg_param[2].abnormal_area_low));
    }
    MainWindow::Pic_Process(2);
}

void MainWindow::on_cam2_src_pic_open_bt_clicked()
{
    QString pic_path = QFileDialog::getOpenFileName(NULL, "选择640x480大小的图片", ".", "image(*.jpeg *.jpg *.png *.bmp)");
    /* 判断是否点了取消操作导致路径为空 */
    if(!QString(pic_path).isEmpty()){
        ui->cam2_src_pic_path_line_edit->setText(pic_path);
        QImage *ori_Image = new QImage(pic_path);
        ScaleImage[2] = ScaleImage2Label(*ori_Image, ui->cam2_source_pic);
        ui->cam2_source_pic->setPixmap(QPixmap::fromImage(ScaleImage[2]));
        ui->cam2_source_pic->setAlignment(Qt::AlignCenter);
        PicLoadFlag[2] = 1;
        MainWindow::Pic_Process(2);
    }
}

void MainWindow::on_cam2_cfg_file_reload_clicked()
{
    Cam_Config_File_Load(2);
    Cam2_Ui_Init();
    MainWindow::Pic_Process(2);
}

void MainWindow::on_cam2_cfg_local_save_clicked()
{
    CFG_PARAM *pCfgParam = &cfg_param[2];
    QString crt_cfg_param = QString("%1 Bin\n%2 Inv\n%3 Otus\n").arg(pCfgParam->bin_threshold).arg(pCfgParam->invert_flag).arg(pCfgParam->otus_flag);
    crt_cfg_param += QString("%1 Ero_Round\n%2 Dia_Round\n").arg(pCfgParam->ero_round).arg(pCfgParam->dia_round);
    crt_cfg_param += QString("%1 PinOut(ms)\n").arg(pCfgParam->pinout_ms);
    crt_cfg_param += QString("%1 Area_Low\n%2 Area_High\n").arg(pCfgParam->abnormal_area_low).arg(pCfgParam->abnormal_area_high);
    crt_cfg_param += QString("%1\n%2\n").arg(pCfgParam->checkarea[0].x).arg(pCfgParam->checkarea[0].y);
    crt_cfg_param += QString("%1\n%2\n").arg(pCfgParam->checkarea[1].x).arg(pCfgParam->checkarea[1].y);
    crt_cfg_param += QString("%1\n%2\n").arg(pCfgParam->checkarea[2].x).arg(pCfgParam->checkarea[2].y);
    crt_cfg_param += QString("%1\n%2\n").arg(pCfgParam->checkarea[3].x).arg(pCfgParam->checkarea[3].y);
    QString fileName = QDir::currentPath()+gCamCfgFile[2].cfg_file_name;
    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream ts(&file);
    ts << crt_cfg_param << endl;
    file.close();
    QMessageBox::about(NULL, "提示", "相机3配置文件保存成功");
}
/* 相机3的相关函数 */
void MainWindow::on_cam3_bin_inv_clicked()
{
    cfg_param[3].invert_flag = ui->cam3_bin_inv->isChecked()?1:0;
    MainWindow::Pic_Process(3);
}

void MainWindow::on_cam3_bin_th_dec_clicked()
{
    cfg_param[3].bin_threshold = cfg_param[3].bin_threshold>0?(cfg_param[3].bin_threshold - 1):0;
    ui->cam3_bin_threshold_slider->setValue(cfg_param[3].bin_threshold);
    ui->cam3_bin_threshold_value->setText(QString::number(cfg_param[3].bin_threshold));
    MainWindow::Pic_Process(3);
}

void MainWindow::on_cam3_bin_th_inc_clicked()
{
    cfg_param[3].bin_threshold = cfg_param[3].bin_threshold<255?(cfg_param[3].bin_threshold + 1):255;
    ui->cam3_bin_threshold_slider->setValue(cfg_param[3].bin_threshold);
    ui->cam3_bin_threshold_value->setText(QString::number(cfg_param[3].bin_threshold));
    MainWindow::Pic_Process(3);
}

void MainWindow::on_cam3_bin_threshold_slider_valueChanged(int value)
{
    cfg_param[3].bin_threshold = value;
    ui->cam3_bin_threshold_value->setText(QString::number(cfg_param[3].bin_threshold));
    MainWindow::Pic_Process(3);
}

void MainWindow::on_cam3_ero_rnd_dec_clicked()
{
    cfg_param[3].ero_round = cfg_param[3].ero_round>0?(cfg_param[3].ero_round - 1):0;
    ui->cam3_ero_round_slider->setValue(cfg_param[3].ero_round);
    ui->cam3_ero_round_value->setText(QString::number(cfg_param[3].ero_round));
    MainWindow::Pic_Process(3);
}

void MainWindow::on_cam3_ero_rnd_inc_clicked()
{
    cfg_param[3].ero_round = cfg_param[3].ero_round<10?(cfg_param[3].ero_round + 1):10;
    ui->cam3_ero_round_slider->setValue(cfg_param[3].ero_round);
    ui->cam3_ero_round_value->setText(QString::number(cfg_param[3].ero_round));
    MainWindow::Pic_Process(3);
}

void MainWindow::on_cam3_ero_round_slider_valueChanged(int value)
{
    cfg_param[3].ero_round = value;
    ui->cam3_ero_round_value->setText(QString::number(cfg_param[3].ero_round));
    MainWindow::Pic_Process(3);
}

void MainWindow::on_cam3_dia_rnd_dec_clicked()
{
    cfg_param[3].dia_round = cfg_param[3].dia_round > 0?(cfg_param[3].dia_round - 1):0;
    ui->cam3_dia_round_slider->setValue(cfg_param[3].dia_round);
    ui->cam3_dia_round_value->setText(QString::number(cfg_param[3].dia_round));
    MainWindow::Pic_Process(3);
}

void MainWindow::on_cam3_dia_rnd_inc_clicked()
{
    cfg_param[3].dia_round = cfg_param[3].dia_round < 10?(cfg_param[3].dia_round + 1):10;
    ui->cam3_dia_round_slider->setValue(cfg_param[3].dia_round);
    ui->cam3_dia_round_value->setText(QString::number(cfg_param[3].dia_round));
    MainWindow::Pic_Process(3);
}

void MainWindow::on_cam3_dia_round_slider_valueChanged(int value)
{
    cfg_param[3].dia_round = value;
    ui->cam3_dia_round_value->setText(QString::number(cfg_param[3].dia_round));
    MainWindow::Pic_Process(3);
}

void MainWindow::on_cam3_ab_area_high_dec_clicked()
{
    int crt_value = cfg_param[3].abnormal_area_high;
    if(crt_value > 0 && crt_value > cfg_param[3].abnormal_area_low){
        cfg_param[3].abnormal_area_high -= 1;
    }else{
        cfg_param[3].abnormal_area_high = 0;
    }
    ui->cam3_abnormal_area_high_slider->setValue(cfg_param[3].abnormal_area_high);
    ui->cam3_abnormal_area_high_value->setText(QString::number(cfg_param[3].abnormal_area_high));
    MainWindow::Pic_Process(3);
}

void MainWindow::on_cam3_ab_area_high_inc_clicked()
{
    int crt_value = cfg_param[3].abnormal_area_high;
    if(crt_value < 300000 && crt_value > cfg_param[3].abnormal_area_low){
        cfg_param[3].abnormal_area_high += 1;
    }else{
        cfg_param[3].abnormal_area_high = 300000;
    }
    ui->cam3_abnormal_area_high_slider->setValue(cfg_param[3].abnormal_area_high);
    ui->cam3_abnormal_area_high_value->setText(QString::number(cfg_param[3].abnormal_area_high));
    MainWindow::Pic_Process(3);
}

void MainWindow::on_cam3_abnormal_area_high_slider_valueChanged(int value)
{
    if(value < cfg_param[3].abnormal_area_low){
        int modify_value = (cfg_param[3].abnormal_area_low+1 < 300000)?(cfg_param[3].abnormal_area_low+1):300000;
        cfg_param[3].abnormal_area_high = modify_value;
        ui->cam3_abnormal_area_high_value->setText(QString::number(cfg_param[3].abnormal_area_high));
        ui->cam3_abnormal_area_high_slider->setValue(cfg_param[3].abnormal_area_high);
    }else{
        cfg_param[3].abnormal_area_high = value;
        ui->cam3_abnormal_area_high_value->setText(QString::number(cfg_param[3].abnormal_area_high));
    }
    MainWindow::Pic_Process(3);
}

void MainWindow::on_cam3_ab_area_low_dec_clicked()
{
    int crt_value = cfg_param[3].abnormal_area_low;
    if(crt_value > 0 && crt_value < cfg_param[3].abnormal_area_high){
        cfg_param[3].abnormal_area_low -= 1;
    }else{
        cfg_param[3].abnormal_area_low = 0;
    }
    ui->cam3_abnormal_area_low_slider->setValue(cfg_param[3].abnormal_area_low);
    ui->cam3_abnormal_area_low_value->setText(QString::number(cfg_param[3].abnormal_area_low));
    MainWindow::Pic_Process(3);
}

void MainWindow::on_cam3_ab_area_low_inc_clicked()
{
    int crt_value = cfg_param[3].abnormal_area_low;
    if(crt_value < 300000 && crt_value < cfg_param[3].abnormal_area_high){
        cfg_param[3].abnormal_area_low += 1;
    }else{
        cfg_param[3].abnormal_area_low = 300000;
    }
    ui->cam3_abnormal_area_low_slider->setValue(cfg_param[3].abnormal_area_low);
    ui->cam3_abnormal_area_low_value->setText(QString::number(cfg_param[3].abnormal_area_low));
    MainWindow::Pic_Process(3);
}

void MainWindow::on_cam3_abnormal_area_low_slider_valueChanged(int value)
{
    if(value > cfg_param[3].abnormal_area_high){
        int modify_value = (cfg_param[3].abnormal_area_high > 1)?(cfg_param[3].abnormal_area_high-1):0;
        cfg_param[3].abnormal_area_low = modify_value;
        ui->cam3_abnormal_area_low_value->setText(QString::number(cfg_param[3].abnormal_area_low));
        ui->cam3_abnormal_area_low_slider->setValue(cfg_param[3].abnormal_area_low);
    }else{
        cfg_param[3].abnormal_area_low = value;
        ui->cam3_abnormal_area_low_value->setText(QString::number(cfg_param[3].abnormal_area_low));
    }
    MainWindow::Pic_Process(3);
}

void MainWindow::on_cam3_src_pic_open_bt_clicked()
{
    QString pic_path = QFileDialog::getOpenFileName(NULL, "选择640x480大小的图片", ".", "image(*.jpeg *.jpg *.png *.bmp)");
    /* 判断是否点了取消操作导致路径为空 */
    if(!QString(pic_path).isEmpty()){
        ui->cam3_src_pic_path_line_edit->setText(pic_path);
        QImage *ori_Image = new QImage(pic_path);
        ScaleImage[3] = ScaleImage2Label(*ori_Image, ui->cam3_source_pic);
        ui->cam3_source_pic->setPixmap(QPixmap::fromImage(ScaleImage[3]));
        ui->cam3_source_pic->setAlignment(Qt::AlignCenter);
        PicLoadFlag[3] = 1;
        MainWindow::Pic_Process(3);
    }
}

void MainWindow::on_cam3_cfg_file_reload_clicked()
{
    Cam_Config_File_Load(3);
    Cam3_Ui_Init();
    MainWindow::Pic_Process(3);
}

void MainWindow::on_cam3_cfg_local_save_clicked()
{
    CFG_PARAM *pCfgParam = &cfg_param[3];
    QString crt_cfg_param = QString("%1 Bin\n%2 Inv\n%3 Otus\n").arg(pCfgParam->bin_threshold).arg(pCfgParam->invert_flag).arg(pCfgParam->otus_flag);
    crt_cfg_param += QString("%1 Ero_Round\n%2 Dia_Round\n").arg(pCfgParam->ero_round).arg(pCfgParam->dia_round);
    crt_cfg_param += QString("%1 PinOut(ms)\n").arg(pCfgParam->pinout_ms);
    crt_cfg_param += QString("%1 Area_Low\n%2 Area_High\n").arg(pCfgParam->abnormal_area_low).arg(pCfgParam->abnormal_area_high);
    crt_cfg_param += QString("%1\n%2\n").arg(pCfgParam->checkarea[0].x).arg(pCfgParam->checkarea[0].y);
    crt_cfg_param += QString("%1\n%2\n").arg(pCfgParam->checkarea[1].x).arg(pCfgParam->checkarea[1].y);
    crt_cfg_param += QString("%1\n%2\n").arg(pCfgParam->checkarea[2].x).arg(pCfgParam->checkarea[2].y);
    crt_cfg_param += QString("%1\n%2\n").arg(pCfgParam->checkarea[3].x).arg(pCfgParam->checkarea[3].y);
    QString fileName = QDir::currentPath()+gCamCfgFile[3].cfg_file_name;
    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream ts(&file);
    ts << crt_cfg_param << endl;
    file.close();
    QMessageBox::about(NULL, "提示", "相机4配置文件保存成功");
}

void MainWindow::on_cam0_abnormal_area_high_value_returnPressed()
{
    QString newvalue = ui->cam0_abnormal_area_high_value->text();
    int new_value = newvalue.toInt();
    if(new_value < 300000 && new_value > cfg_param[0].abnormal_area_low){
        cfg_param[0].abnormal_area_high = new_value;
    }
    ui->cam0_abnormal_area_high_slider->setValue(cfg_param[0].abnormal_area_high);
    MainWindow::Pic_Process(0);
}

void MainWindow::on_cam0_abnormal_area_low_value_returnPressed()
{
    QString newvalue = ui->cam0_abnormal_area_low_value->text();
    int new_value = newvalue.toInt();
    if(new_value > 0 && new_value < cfg_param[0].abnormal_area_high){
        cfg_param[0].abnormal_area_low = new_value;
    }
    ui->cam0_abnormal_area_low_slider->setValue(cfg_param[0].abnormal_area_low);
    MainWindow::Pic_Process(0);
}

void MainWindow::on_cam1_abnormal_area_high_value_returnPressed()
{
    QString newvalue = ui->cam1_abnormal_area_high_value->text();
    int new_value = newvalue.toInt();
    if(new_value < 300000 && new_value > cfg_param[1].abnormal_area_low){
        cfg_param[1].abnormal_area_high = new_value;
    }
    ui->cam1_abnormal_area_high_slider->setValue(cfg_param[1].abnormal_area_high);
    MainWindow::Pic_Process(1);
}

void MainWindow::on_cam1_abnormal_area_low_value_returnPressed()
{
    QString newvalue = ui->cam1_abnormal_area_low_value->text();
    int new_value = newvalue.toInt();
    if(new_value > 0 && new_value < cfg_param[1].abnormal_area_high){
        cfg_param[1].abnormal_area_low = new_value;
    }
    ui->cam1_abnormal_area_low_slider->setValue(cfg_param[1].abnormal_area_low);
    MainWindow::Pic_Process(1);
}

void MainWindow::on_cam2_abnormal_area_high_value_returnPressed()
{
    QString newvalue = ui->cam2_abnormal_area_high_value->text();
    int new_value = newvalue.toInt();
    if(new_value < 300000 && new_value > cfg_param[2].abnormal_area_low){
        cfg_param[2].abnormal_area_high = new_value;
    }
    ui->cam2_abnormal_area_high_slider->setValue(cfg_param[2].abnormal_area_high);
    MainWindow::Pic_Process(2);
}

void MainWindow::on_cam2_abnormal_area_low_value_returnPressed()
{
    QString newvalue = ui->cam2_abnormal_area_low_value->text();
    int new_value = newvalue.toInt();
    if(new_value > 0 && new_value < cfg_param[2].abnormal_area_high){
        cfg_param[2].abnormal_area_low = new_value;
    }
    ui->cam2_abnormal_area_low_slider->setValue(cfg_param[2].abnormal_area_low);
    MainWindow::Pic_Process(2);
}

void MainWindow::on_cam3_abnormal_area_high_value_returnPressed()
{
    QString newvalue = ui->cam3_abnormal_area_high_value->text();
    int new_value = newvalue.toInt();
    if(new_value < 300000 && new_value > cfg_param[3].abnormal_area_low){
        cfg_param[3].abnormal_area_high = new_value;
    }
    ui->cam3_abnormal_area_high_slider->setValue(cfg_param[3].abnormal_area_high);
    MainWindow::Pic_Process(3);
}

void MainWindow::on_cam3_abnormal_area_low_value_returnPressed()
{
    QString newvalue = ui->cam3_abnormal_area_low_value->text();
    int new_value = newvalue.toInt();
    if(new_value > 0 && new_value < cfg_param[3].abnormal_area_high){
        cfg_param[3].abnormal_area_low = new_value;
    }
    ui->cam3_abnormal_area_low_slider->setValue(cfg_param[3].abnormal_area_low);
    MainWindow::Pic_Process(3);
}

void MainWindow::on_cam0_check_area_p1_clicked()
{
    AreaSelectActivePoint[0] = 0;
    ui->cam0_check_area_p1->setChecked(TRUE);
    ui->cam0_check_area_p2->setChecked(FALSE);
    ui->cam0_check_area_p3->setChecked(FALSE);
    ui->cam0_check_area_p4->setChecked(FALSE);
    ui->cam0_check_area_point_hslider->setValue(cfg_param[0].checkarea[0].x);
    ui->cam0_check_area_point_vslider->setValue(cfg_param[0].checkarea[0].y);
}

void MainWindow::on_cam0_check_area_p2_clicked()
{
    AreaSelectActivePoint[0] = 1;
    ui->cam0_check_area_p1->setChecked(FALSE);
    ui->cam0_check_area_p2->setChecked(TRUE);
    ui->cam0_check_area_p3->setChecked(FALSE);
    ui->cam0_check_area_p4->setChecked(FALSE);
    ui->cam0_check_area_point_hslider->setValue(cfg_param[0].checkarea[1].x);
    ui->cam0_check_area_point_vslider->setValue(cfg_param[0].checkarea[1].y);
}

void MainWindow::on_cam0_check_area_p3_clicked()
{
    AreaSelectActivePoint[0] = 2;
    ui->cam0_check_area_p1->setChecked(FALSE);
    ui->cam0_check_area_p2->setChecked(FALSE);
    ui->cam0_check_area_p3->setChecked(TRUE);
    ui->cam0_check_area_p4->setChecked(FALSE);
    ui->cam0_check_area_point_hslider->setValue(cfg_param[0].checkarea[2].x);
    ui->cam0_check_area_point_vslider->setValue(cfg_param[0].checkarea[2].y);
}

void MainWindow::on_cam0_check_area_p4_clicked()
{
    AreaSelectActivePoint[0] = 3;
    ui->cam0_check_area_p1->setChecked(FALSE);
    ui->cam0_check_area_p2->setChecked(FALSE);
    ui->cam0_check_area_p3->setChecked(FALSE);
    ui->cam0_check_area_p4->setChecked(TRUE);
    ui->cam0_check_area_point_hslider->setValue(cfg_param[0].checkarea[3].x);
    ui->cam0_check_area_point_vslider->setValue(cfg_param[0].checkarea[3].y);
}

void MainWindow::on_cam1_check_area_p1_clicked()
{
    AreaSelectActivePoint[1] = 0;
    ui->cam1_check_area_p1->setChecked(TRUE);
    ui->cam1_check_area_p2->setChecked(FALSE);
    ui->cam1_check_area_p3->setChecked(FALSE);
    ui->cam1_check_area_p4->setChecked(FALSE);
    ui->cam1_check_area_point_hslider->setValue(cfg_param[1].checkarea[0].x);
    ui->cam1_check_area_point_vslider->setValue(cfg_param[1].checkarea[0].y);
}

void MainWindow::on_cam1_check_area_p2_clicked()
{
    AreaSelectActivePoint[1] = 1;
    ui->cam1_check_area_p1->setChecked(FALSE);
    ui->cam1_check_area_p2->setChecked(TRUE);
    ui->cam1_check_area_p3->setChecked(FALSE);
    ui->cam1_check_area_p4->setChecked(FALSE);
    ui->cam1_check_area_point_hslider->setValue(cfg_param[1].checkarea[1].x);
    ui->cam1_check_area_point_vslider->setValue(cfg_param[1].checkarea[1].y);
}

void MainWindow::on_cam1_check_area_p3_clicked()
{
    AreaSelectActivePoint[1] = 2;
    ui->cam1_check_area_p1->setChecked(FALSE);
    ui->cam1_check_area_p2->setChecked(FALSE);
    ui->cam1_check_area_p3->setChecked(TRUE);
    ui->cam1_check_area_p4->setChecked(FALSE);
    ui->cam1_check_area_point_hslider->setValue(cfg_param[1].checkarea[2].x);
    ui->cam1_check_area_point_vslider->setValue(cfg_param[1].checkarea[2].y);
}

void MainWindow::on_cam1_check_area_p4_clicked()
{
    AreaSelectActivePoint[1] = 3;
    ui->cam1_check_area_p1->setChecked(FALSE);
    ui->cam1_check_area_p2->setChecked(FALSE);
    ui->cam1_check_area_p3->setChecked(FALSE);
    ui->cam1_check_area_p4->setChecked(TRUE);
    ui->cam1_check_area_point_hslider->setValue(cfg_param[1].checkarea[3].x);
    ui->cam1_check_area_point_vslider->setValue(cfg_param[1].checkarea[3].y);
}


void MainWindow::on_cam2_check_area_p1_clicked()
{
    AreaSelectActivePoint[2] = 0;
    ui->cam2_check_area_p1->setChecked(TRUE);
    ui->cam2_check_area_p2->setChecked(FALSE);
    ui->cam2_check_area_p3->setChecked(FALSE);
    ui->cam2_check_area_p4->setChecked(FALSE);
    ui->cam2_check_area_point_hslider->setValue(cfg_param[2].checkarea[0].x);
    ui->cam2_check_area_point_vslider->setValue(cfg_param[2].checkarea[0].y);
}

void MainWindow::on_cam2_check_area_p2_clicked()
{
    AreaSelectActivePoint[2] = 1;
    ui->cam2_check_area_p1->setChecked(FALSE);
    ui->cam2_check_area_p2->setChecked(TRUE);
    ui->cam2_check_area_p3->setChecked(FALSE);
    ui->cam2_check_area_p4->setChecked(FALSE);
    ui->cam2_check_area_point_hslider->setValue(cfg_param[2].checkarea[1].x);
    ui->cam2_check_area_point_vslider->setValue(cfg_param[2].checkarea[1].y);
}

void MainWindow::on_cam2_check_area_p3_clicked()
{
    AreaSelectActivePoint[2] = 2;
    ui->cam2_check_area_p1->setChecked(FALSE);
    ui->cam2_check_area_p2->setChecked(FALSE);
    ui->cam2_check_area_p3->setChecked(TRUE);
    ui->cam2_check_area_p4->setChecked(FALSE);
    ui->cam2_check_area_point_hslider->setValue(cfg_param[2].checkarea[2].x);
    ui->cam2_check_area_point_vslider->setValue(cfg_param[2].checkarea[2].y);
}

void MainWindow::on_cam2_check_area_p4_clicked()
{
    AreaSelectActivePoint[2] = 3;
    ui->cam2_check_area_p1->setChecked(FALSE);
    ui->cam2_check_area_p2->setChecked(FALSE);
    ui->cam2_check_area_p3->setChecked(FALSE);
    ui->cam2_check_area_p4->setChecked(TRUE);
    ui->cam2_check_area_point_hslider->setValue(cfg_param[2].checkarea[3].x);
    ui->cam2_check_area_point_vslider->setValue(cfg_param[2].checkarea[3].y);
}


void MainWindow::on_cam3_check_area_p1_clicked()
{
    AreaSelectActivePoint[3] = 0;
    ui->cam3_check_area_p1->setChecked(TRUE);
    ui->cam3_check_area_p2->setChecked(FALSE);
    ui->cam3_check_area_p3->setChecked(FALSE);
    ui->cam3_check_area_p4->setChecked(FALSE);
    ui->cam3_check_area_point_hslider->setValue(cfg_param[3].checkarea[0].x);
    ui->cam3_check_area_point_vslider->setValue(cfg_param[3].checkarea[0].y);
}

void MainWindow::on_cam3_check_area_p2_clicked()
{
    AreaSelectActivePoint[3] = 1;
    ui->cam3_check_area_p1->setChecked(FALSE);
    ui->cam3_check_area_p2->setChecked(TRUE);
    ui->cam3_check_area_p3->setChecked(FALSE);
    ui->cam3_check_area_p4->setChecked(FALSE);
    ui->cam3_check_area_point_hslider->setValue(cfg_param[3].checkarea[1].x);
    ui->cam3_check_area_point_vslider->setValue(cfg_param[3].checkarea[1].y);
}

void MainWindow::on_cam3_check_area_p3_clicked()
{
    AreaSelectActivePoint[3] = 2;
    ui->cam3_check_area_p1->setChecked(FALSE);
    ui->cam3_check_area_p2->setChecked(FALSE);
    ui->cam3_check_area_p3->setChecked(TRUE);
    ui->cam3_check_area_p4->setChecked(FALSE);
    ui->cam3_check_area_point_hslider->setValue(cfg_param[3].checkarea[2].x);
    ui->cam3_check_area_point_vslider->setValue(cfg_param[3].checkarea[2].y);
}

void MainWindow::on_cam3_check_area_p4_clicked()
{
    AreaSelectActivePoint[3] = 3;
    ui->cam3_check_area_p1->setChecked(FALSE);
    ui->cam3_check_area_p2->setChecked(FALSE);
    ui->cam3_check_area_p3->setChecked(FALSE);
    ui->cam3_check_area_p4->setChecked(TRUE);
    ui->cam3_check_area_point_hslider->setValue(cfg_param[3].checkarea[3].x);
    ui->cam3_check_area_point_vslider->setValue(cfg_param[3].checkarea[3].y);
}

void MainWindow::on_cam0_check_area_point_up_clicked()
{
    int activePoint = AreaSelectActivePoint[0];
    CFG_PARAM *pCfgParam = &cfg_param[0];
    if(pCfgParam->checkarea[activePoint].y > 1){
        pCfgParam->checkarea[activePoint].y -= 1;
        ui->cam0_check_area_point_vslider->setValue(pCfgParam->checkarea[activePoint].y);
        MainWindow::Pic_Process(0);
    }
}

void MainWindow::on_cam0_check_area_point_down_clicked()
{
    int activePoint = AreaSelectActivePoint[0];
    CFG_PARAM *pCfgParam = &cfg_param[0];
    if(pCfgParam->checkarea[activePoint].y < 479){
        pCfgParam->checkarea[activePoint].y += 1;
        ui->cam0_check_area_point_vslider->setValue(pCfgParam->checkarea[activePoint].y);
        MainWindow::Pic_Process(0);
    }
}

void MainWindow::on_cam0_check_area_point_left_clicked()
{
    int activePoint = AreaSelectActivePoint[0];
    CFG_PARAM *pCfgParam = &cfg_param[0];
    if(pCfgParam->checkarea[activePoint].x > 1){
        pCfgParam->checkarea[activePoint].x -= 1;
        ui->cam0_check_area_point_hslider->setValue(pCfgParam->checkarea[activePoint].x);
        MainWindow::Pic_Process(0);
    }
}

void MainWindow::on_cam0_check_area_point_right_clicked()
{
    int activePoint = AreaSelectActivePoint[0];
    CFG_PARAM *pCfgParam = &cfg_param[0];
    if(pCfgParam->checkarea[activePoint].x < 639){
        pCfgParam->checkarea[activePoint].x += 1;
        ui->cam0_check_area_point_hslider->setValue(pCfgParam->checkarea[activePoint].x);
        MainWindow::Pic_Process(0);
    }
}

void MainWindow::on_cam1_check_area_point_up_clicked()
{
    int activePoint = AreaSelectActivePoint[0];
    CFG_PARAM *pCfgParam = &cfg_param[0];
    if(pCfgParam->checkarea[activePoint].y > 1){
        pCfgParam->checkarea[activePoint].y -= 1;
        ui->cam0_check_area_point_vslider->setValue(pCfgParam->checkarea[activePoint].y);
        MainWindow::Pic_Process(0);
    }
}

void MainWindow::on_cam1_check_area_point_down_clicked()
{
    int activePoint = AreaSelectActivePoint[1];
    CFG_PARAM *pCfgParam = &cfg_param[1];
    if(pCfgParam->checkarea[activePoint].y < 479){
        pCfgParam->checkarea[activePoint].y += 1;
        ui->cam1_check_area_point_vslider->setValue(pCfgParam->checkarea[activePoint].y);
        MainWindow::Pic_Process(1);
    }
}

void MainWindow::on_cam1_check_area_point_left_clicked()
{
    int activePoint = AreaSelectActivePoint[1];
    CFG_PARAM *pCfgParam = &cfg_param[1];
    if(pCfgParam->checkarea[activePoint].x > 1){
        pCfgParam->checkarea[activePoint].x -= 1;
        ui->cam1_check_area_point_hslider->setValue(pCfgParam->checkarea[activePoint].x);
        MainWindow::Pic_Process(1);
    }
}

void MainWindow::on_cam1_check_area_point_right_clicked()
{
    int activePoint = AreaSelectActivePoint[1];
    CFG_PARAM *pCfgParam = &cfg_param[1];
    if(pCfgParam->checkarea[activePoint].x < 639){
        pCfgParam->checkarea[activePoint].x += 1;
        ui->cam1_check_area_point_hslider->setValue(pCfgParam->checkarea[activePoint].x);
        MainWindow::Pic_Process(1);
    }
}

void MainWindow::on_cam2_check_area_point_up_clicked()
{
    int activePoint = AreaSelectActivePoint[2];
    CFG_PARAM *pCfgParam = &cfg_param[2];
    if(pCfgParam->checkarea[activePoint].y > 1){
        pCfgParam->checkarea[activePoint].y -= 1;
        ui->cam2_check_area_point_vslider->setValue(pCfgParam->checkarea[activePoint].y);
        MainWindow::Pic_Process(2);
    }
}

void MainWindow::on_cam2_check_area_point_down_clicked()
{
    int activePoint = AreaSelectActivePoint[2];
    CFG_PARAM *pCfgParam = &cfg_param[2];
    if(pCfgParam->checkarea[activePoint].y < 479){
        pCfgParam->checkarea[activePoint].y += 1;
        ui->cam2_check_area_point_vslider->setValue(pCfgParam->checkarea[activePoint].y);
        MainWindow::Pic_Process(2);
    }
}

void MainWindow::on_cam2_check_area_point_left_clicked()
{
    int activePoint = AreaSelectActivePoint[2];
    CFG_PARAM *pCfgParam = &cfg_param[2];
    if(pCfgParam->checkarea[activePoint].x > 1){
        pCfgParam->checkarea[activePoint].x -= 1;
        ui->cam2_check_area_point_hslider->setValue(pCfgParam->checkarea[activePoint].x);
        MainWindow::Pic_Process(2);
    }
}

void MainWindow::on_cam2_check_area_point_right_clicked()
{
    int activePoint = AreaSelectActivePoint[2];
    CFG_PARAM *pCfgParam = &cfg_param[2];
    if(pCfgParam->checkarea[activePoint].x < 639){
        pCfgParam->checkarea[activePoint].x += 1;
        ui->cam2_check_area_point_hslider->setValue(pCfgParam->checkarea[activePoint].x);
        MainWindow::Pic_Process(2);
    }
}

void MainWindow::on_cam3_check_area_point_up_clicked()
{
    int activePoint = AreaSelectActivePoint[3];
    CFG_PARAM *pCfgParam = &cfg_param[3];
    if(pCfgParam->checkarea[activePoint].y > 1){
        pCfgParam->checkarea[activePoint].y -= 1;
        ui->cam3_check_area_point_vslider->setValue(pCfgParam->checkarea[activePoint].y);
        MainWindow::Pic_Process(3);
    }
}

void MainWindow::on_cam3_check_area_point_down_clicked()
{
    int activePoint = AreaSelectActivePoint[3];
    CFG_PARAM *pCfgParam = &cfg_param[3];
    if(pCfgParam->checkarea[activePoint].y < 479){
        pCfgParam->checkarea[activePoint].y += 1;
        ui->cam3_check_area_point_vslider->setValue(pCfgParam->checkarea[activePoint].y);
        MainWindow::Pic_Process(3);
    }
}

void MainWindow::on_cam3_check_area_point_left_clicked()
{
    int activePoint = AreaSelectActivePoint[3];
    CFG_PARAM *pCfgParam = &cfg_param[3];
    if(pCfgParam->checkarea[activePoint].x > 1){
        pCfgParam->checkarea[activePoint].x -= 1;
        ui->cam3_check_area_point_hslider->setValue(pCfgParam->checkarea[activePoint].x);
        MainWindow::Pic_Process(3);
    }
}

void MainWindow::on_cam3_check_area_point_right_clicked()
{
    int activePoint = AreaSelectActivePoint[3];
    CFG_PARAM *pCfgParam = &cfg_param[3];
    if(pCfgParam->checkarea[activePoint].x < 639){
        pCfgParam->checkarea[activePoint].x += 1;
        ui->cam3_check_area_point_hslider->setValue(pCfgParam->checkarea[activePoint].x);
        MainWindow::Pic_Process(3);
    }
}

void MainWindow::on_cam0_check_area_point_hslider_valueChanged(int value)
{
    int cam_id = 0;
    int activePoint = AreaSelectActivePoint[cam_id];
    CFG_PARAM *pCfgParam = &cfg_param[cam_id];
    pCfgParam->checkarea[activePoint].x = value;
    ui->cam0_check_area_point_hslider->setValue(value);
    MainWindow::Pic_Process(cam_id);
}

void MainWindow::on_cam0_check_area_point_vslider_valueChanged(int value)
{
    int cam_id = 0;
    int activePoint = AreaSelectActivePoint[cam_id];
    CFG_PARAM *pCfgParam = &cfg_param[cam_id];
    pCfgParam->checkarea[activePoint].y = value;
    ui->cam0_check_area_point_vslider->setValue(value);
    MainWindow::Pic_Process(cam_id);
}

void MainWindow::on_cam1_check_area_point_hslider_valueChanged(int value)
{
    int cam_id = 1;
    int activePoint = AreaSelectActivePoint[cam_id];
    CFG_PARAM *pCfgParam = &cfg_param[cam_id];
    pCfgParam->checkarea[activePoint].x = value;
    ui->cam1_check_area_point_hslider->setValue(value);
    MainWindow::Pic_Process(cam_id);
}

void MainWindow::on_cam1_check_area_point_vslider_valueChanged(int value)
{
    int cam_id = 1;
    int activePoint = AreaSelectActivePoint[cam_id];
    CFG_PARAM *pCfgParam = &cfg_param[cam_id];
    pCfgParam->checkarea[activePoint].y = value;
    ui->cam1_check_area_point_vslider->setValue(value);
    MainWindow::Pic_Process(cam_id);
}

void MainWindow::on_cam2_check_area_point_hslider_valueChanged(int value)
{
    int cam_id = 2;
    int activePoint = AreaSelectActivePoint[cam_id];
    CFG_PARAM *pCfgParam = &cfg_param[cam_id];
    pCfgParam->checkarea[activePoint].x = value;
    ui->cam2_check_area_point_hslider->setValue(value);
    MainWindow::Pic_Process(cam_id);
}

void MainWindow::on_cam2_check_area_point_vslider_valueChanged(int value)
{
    int cam_id = 2;
    int activePoint = AreaSelectActivePoint[cam_id];
    CFG_PARAM *pCfgParam = &cfg_param[cam_id];
    pCfgParam->checkarea[activePoint].y = value;
    ui->cam2_check_area_point_vslider->setValue(value);
    MainWindow::Pic_Process(cam_id);
}
void MainWindow::on_cam3_check_area_point_hslider_valueChanged(int value)
{
    int cam_id = 3;
    int activePoint = AreaSelectActivePoint[cam_id];
    CFG_PARAM *pCfgParam = &cfg_param[cam_id];
    pCfgParam->checkarea[activePoint].x = value;
    ui->cam3_check_area_point_hslider->setValue(value);
    MainWindow::Pic_Process(cam_id);
}

void MainWindow::on_cam3_check_area_point_vslider_valueChanged(int value)
{
    int cam_id = 3;
    int activePoint = AreaSelectActivePoint[cam_id];
    CFG_PARAM *pCfgParam = &cfg_param[cam_id];
    pCfgParam->checkarea[activePoint].y = value;
    ui->cam3_check_area_point_vslider->setValue(value);
    MainWindow::Pic_Process(cam_id);
}

void MainWindow::on_cam_gpio_output_pluse_width_valueChanged(int value)
{
    cfg_param[0].pinout_ms = value;
    cfg_param[1].pinout_ms = value;
    cfg_param[2].pinout_ms = value;
    cfg_param[3].pinout_ms = value;
    ui->cam_gpio_pluse_width->setText(QString::number(value)+"ms");
}

void MainWindow::on_cam_gpio_pluse_width_returnPressed()
{
    QString newvalue = ui->cam_gpio_pluse_width->text();
    int new_value = newvalue.toInt();
    cfg_param[0].pinout_ms = new_value;
    cfg_param[1].pinout_ms = new_value;
    cfg_param[2].pinout_ms = new_value;
    cfg_param[3].pinout_ms = new_value;
    ui->cam_gpio_output_pluse_width->setValue(new_value);
}
/* 相机0通道选择 */
void MainWindow::on_cam0_color_channel_clicked()
{
    int cam_id = 0;
    CFG_PARAM *pCfgParam = &cfg_param[cam_id];
    pCfgParam->channel = 0;
    ui->cam0_color_channel->setChecked(TRUE);
    ui->cam0_r_channel->setChecked(FALSE);
    ui->cam0_g_channel->setChecked(FALSE);
    ui->cam0_b_channel->setChecked(FALSE);
    MainWindow::Pic_Process(cam_id);
}

void MainWindow::on_cam0_r_channel_clicked()
{
    int cam_id = 0;
    CFG_PARAM *pCfgParam = &cfg_param[cam_id];
    pCfgParam->channel = 1;
    ui->cam0_color_channel->setChecked(FALSE);
    ui->cam0_r_channel->setChecked(TRUE);
    ui->cam0_g_channel->setChecked(FALSE);
    ui->cam0_b_channel->setChecked(FALSE);
    MainWindow::Pic_Process(cam_id);
}

void MainWindow::on_cam0_g_channel_clicked()
{
    int cam_id = 0;
    CFG_PARAM *pCfgParam = &cfg_param[cam_id];
    pCfgParam->channel = 2;
    ui->cam0_color_channel->setChecked(FALSE);
    ui->cam0_r_channel->setChecked(FALSE);
    ui->cam0_g_channel->setChecked(TRUE);
    ui->cam0_b_channel->setChecked(FALSE);
    MainWindow::Pic_Process(cam_id);
}

void MainWindow::on_cam0_b_channel_clicked()
{
    int cam_id = 0;
    CFG_PARAM *pCfgParam = &cfg_param[cam_id];
    pCfgParam->channel = 3;
    ui->cam0_color_channel->setChecked(FALSE);
    ui->cam0_r_channel->setChecked(FALSE);
    ui->cam0_g_channel->setChecked(FALSE);
    ui->cam0_b_channel->setChecked(TRUE);
    MainWindow::Pic_Process(cam_id);
}

/* 相机1通道选择 */
void MainWindow::on_cam1_color_channel_clicked()
{
    int cam_id = 1;
    CFG_PARAM *pCfgParam = &cfg_param[cam_id];
    pCfgParam->channel = 0;
    ui->cam1_color_channel->setChecked(TRUE);
    ui->cam1_r_channel->setChecked(FALSE);
    ui->cam1_g_channel->setChecked(FALSE);
    ui->cam1_b_channel->setChecked(FALSE);
    MainWindow::Pic_Process(cam_id);
}

void MainWindow::on_cam1_r_channel_clicked()
{
    int cam_id = 1;
    CFG_PARAM *pCfgParam = &cfg_param[cam_id];
    pCfgParam->channel = 1;
    ui->cam1_color_channel->setChecked(FALSE);
    ui->cam1_r_channel->setChecked(TRUE);
    ui->cam1_g_channel->setChecked(FALSE);
    ui->cam1_b_channel->setChecked(FALSE);
    MainWindow::Pic_Process(cam_id);
}

void MainWindow::on_cam1_g_channel_clicked()
{
    int cam_id = 1;
    CFG_PARAM *pCfgParam = &cfg_param[cam_id];
    pCfgParam->channel = 2;
    ui->cam1_color_channel->setChecked(FALSE);
    ui->cam1_r_channel->setChecked(FALSE);
    ui->cam1_g_channel->setChecked(TRUE);
    ui->cam1_b_channel->setChecked(FALSE);
    MainWindow::Pic_Process(cam_id);
}

void MainWindow::on_cam1_b_channel_clicked()
{
    int cam_id = 1;
    CFG_PARAM *pCfgParam = &cfg_param[cam_id];
    pCfgParam->channel = 3;
    ui->cam1_color_channel->setChecked(FALSE);
    ui->cam1_r_channel->setChecked(FALSE);
    ui->cam1_g_channel->setChecked(FALSE);
    ui->cam1_b_channel->setChecked(TRUE);
    MainWindow::Pic_Process(cam_id);
}

/* 相机2通道选择 */
void MainWindow::on_cam2_color_channel_clicked()
{
    int cam_id = 2;
    CFG_PARAM *pCfgParam = &cfg_param[cam_id];
    pCfgParam->channel = 2;
    ui->cam2_color_channel->setChecked(TRUE);
    ui->cam2_r_channel->setChecked(FALSE);
    ui->cam2_g_channel->setChecked(FALSE);
    ui->cam2_b_channel->setChecked(FALSE);
    MainWindow::Pic_Process(cam_id);
}

void MainWindow::on_cam2_r_channel_clicked()
{
    int cam_id = 2;
    CFG_PARAM *pCfgParam = &cfg_param[cam_id];
    pCfgParam->channel = 1;
    ui->cam2_color_channel->setChecked(FALSE);
    ui->cam2_r_channel->setChecked(TRUE);
    ui->cam2_g_channel->setChecked(FALSE);
    ui->cam2_b_channel->setChecked(FALSE);
    MainWindow::Pic_Process(cam_id);
}

void MainWindow::on_cam2_g_channel_clicked()
{
    int cam_id = 2;
    CFG_PARAM *pCfgParam = &cfg_param[cam_id];
    pCfgParam->channel = 2;
    ui->cam2_color_channel->setChecked(FALSE);
    ui->cam2_r_channel->setChecked(FALSE);
    ui->cam2_g_channel->setChecked(TRUE);
    ui->cam2_b_channel->setChecked(FALSE);
    MainWindow::Pic_Process(cam_id);
}

void MainWindow::on_cam2_b_channel_clicked()
{
    int cam_id = 2;
    CFG_PARAM *pCfgParam = &cfg_param[cam_id];
    pCfgParam->channel = 3;
    ui->cam2_color_channel->setChecked(FALSE);
    ui->cam2_r_channel->setChecked(FALSE);
    ui->cam2_g_channel->setChecked(FALSE);
    ui->cam2_b_channel->setChecked(TRUE);
    MainWindow::Pic_Process(cam_id);
}

/* 相机3通道选择 */
void MainWindow::on_cam3_color_channel_clicked()
{
    int cam_id = 3;
    CFG_PARAM *pCfgParam = &cfg_param[cam_id];
    pCfgParam->channel = 0;
    ui->cam3_color_channel->setChecked(TRUE);
    ui->cam3_r_channel->setChecked(FALSE);
    ui->cam3_g_channel->setChecked(FALSE);
    ui->cam3_b_channel->setChecked(FALSE);
    MainWindow::Pic_Process(cam_id);
}

void MainWindow::on_cam3_r_channel_clicked()
{
    int cam_id = 3;
    CFG_PARAM *pCfgParam = &cfg_param[cam_id];
    pCfgParam->channel = 1;
    ui->cam3_color_channel->setChecked(FALSE);
    ui->cam3_r_channel->setChecked(TRUE);
    ui->cam3_g_channel->setChecked(FALSE);
    ui->cam3_b_channel->setChecked(FALSE);
    MainWindow::Pic_Process(cam_id);
}

void MainWindow::on_cam3_g_channel_clicked()
{
    int cam_id = 3;
    CFG_PARAM *pCfgParam = &cfg_param[cam_id];
    pCfgParam->channel = 2;
    ui->cam3_color_channel->setChecked(FALSE);
    ui->cam3_r_channel->setChecked(FALSE);
    ui->cam3_g_channel->setChecked(TRUE);
    ui->cam3_b_channel->setChecked(FALSE);
    MainWindow::Pic_Process(cam_id);
}

void MainWindow::on_cam3_b_channel_clicked()
{
    int cam_id = 3;
    CFG_PARAM *pCfgParam = &cfg_param[cam_id];
    pCfgParam->channel = 3;
    ui->cam3_color_channel->setChecked(FALSE);
    ui->cam3_r_channel->setChecked(FALSE);
    ui->cam3_g_channel->setChecked(FALSE);
    ui->cam3_b_channel->setChecked(TRUE);
    MainWindow::Pic_Process(cam_id);
}

void MainWindow::on_cam0_otus_binary_clicked()
{
    cfg_param[0].otus_flag = ui->cam0_otus_binary->isChecked()?1:0;
    ui->cam0_bin_threshold_slider->setEnabled(cfg_param[0].otus_flag?FALSE:TRUE);
    ui->cam0_bin_th_dec->setEnabled(cfg_param[0].otus_flag?FALSE:TRUE);
    ui->cam0_bin_th_inc->setEnabled(cfg_param[0].otus_flag?FALSE:TRUE);
    ui->cam0_bin_threshold_value->setEnabled(cfg_param[0].otus_flag?FALSE:TRUE);

    MainWindow::Pic_Process(0);
}

void MainWindow::on_cam1_otus_binary_clicked()
{
    cfg_param[1].otus_flag = ui->cam1_otus_binary->isChecked()?1:0;
    ui->cam1_bin_threshold_slider->setEnabled(cfg_param[1].otus_flag?FALSE:TRUE);
    ui->cam1_bin_th_dec->setEnabled(cfg_param[1].otus_flag?FALSE:TRUE);
    ui->cam1_bin_th_inc->setEnabled(cfg_param[1].otus_flag?FALSE:TRUE);
    ui->cam1_bin_threshold_value->setEnabled(cfg_param[1].otus_flag?FALSE:TRUE);
    MainWindow::Pic_Process(1);
}

void MainWindow::on_cam2_otus_binary_clicked()
{
    cfg_param[2].otus_flag = ui->cam2_otus_binary->isChecked()?1:0;
    ui->cam2_bin_threshold_slider->setEnabled(cfg_param[2].otus_flag?FALSE:TRUE);
    ui->cam2_bin_th_dec->setEnabled(cfg_param[2].otus_flag?FALSE:TRUE);
    ui->cam2_bin_th_inc->setEnabled(cfg_param[2].otus_flag?FALSE:TRUE);
    ui->cam2_bin_threshold_value->setEnabled(cfg_param[2].otus_flag?FALSE:TRUE);
    MainWindow::Pic_Process(2);
}

void MainWindow::on_cam3_otus_binary_clicked()
{
    cfg_param[3].otus_flag = ui->cam3_otus_binary->isChecked()?1:0;
    ui->cam3_bin_threshold_slider->setEnabled(cfg_param[3].otus_flag?FALSE:TRUE);
    ui->cam3_bin_th_dec->setEnabled(cfg_param[3].otus_flag?FALSE:TRUE);
    ui->cam3_bin_th_inc->setEnabled(cfg_param[3].otus_flag?FALSE:TRUE);
    ui->cam3_bin_threshold_value->setEnabled(cfg_param[3].otus_flag?FALSE:TRUE);
    MainWindow::Pic_Process(3);
}

void MainWindow::on_actioncam0_cfg_open_triggered()
{
    on_cam0_cfg_file_reload_clicked();
}

void MainWindow::on_actioncam0_cfg_save_triggered()
{
    on_cam0_cfg_local_save_clicked();
}

void MainWindow::on_actioncam1_cfg_open_triggered()
{
    on_cam1_cfg_file_reload_clicked();
}

void MainWindow::on_actioncam1_cfg_save_triggered()
{
    on_cam1_cfg_local_save_clicked();
}

void MainWindow::on_actioncam2_cfg_open_triggered()
{
    on_cam2_cfg_file_reload_clicked();
}

void MainWindow::on_actioncam2_cfg_save_triggered()
{
    on_cam2_cfg_local_save_clicked();
}

void MainWindow::on_actioncam3_cfg_open_triggered()
{
    on_cam3_cfg_file_reload_clicked();
}

void MainWindow::on_actioncam3_cfg_save_triggered()
{
    on_cam3_cfg_local_save_clicked();
}
