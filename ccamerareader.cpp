#include "ccamerareader.h"
#include "videoio.hpp"
#include "imgproc.hpp"

#include <QImage>
#include <QString>
#include <QDebug>
#include <QPixmap>

using namespace cv;

CCameraReader::CCameraReader() :
    mp_camera(new VideoCapture)
{
}

CCameraReader::~CCameraReader()
{
}

const Mat CCameraReader::getFrame()
{
    Mat frame;
    *mp_camera >> frame;

//    // make gray img, and increase contrast
//    cvtColor(frame, grayFrame, CV_BGR2GRAY);
//    equalizeHist(grayFrame, grayFrame);

    return frame;
}

void CCameraReader::openCamera()
{
    if (mp_camera->isOpened())
        throw QString("Camera opened without close in other function.");

    if (!mp_camera->open(0))
        throw QString("Camera opened failed.");

}

void CCameraReader::closeCamera()
{
    try {
        mp_camera->release();
    } catch(const Exception &e)
    {
        qDebug() << "CCameraReader::closeCamera : cannot release camera";
        qDebug() << e.what();
    }
}

void CCameraReader::startDispCamera()
{
    try {
        openCamera();
    } catch(const QString &e)
    {
        qDebug() << "CCameraReader::startDispCamera : can not open camera.";
        throw e;
    }

    mp_timer = new QTimer;
    connect(mp_timer, &QTimer::timeout, this, &CCameraReader::timeOut);
    mp_timer->start(1000);
}

void CCameraReader::stopDispCamera()
{
    mp_timer->stop();
    disconnect(mp_timer, &QTimer::timeout, this, &CCameraReader::timeOut);
    mp_timer->~QTimer();
}

void CCameraReader::timeOut()
{
    Mat frame = getFrame();
    QImage image;
    try{
        image = Mat2QImage(frame);
    } catch(const QString &e)
    {
        qDebug() << "CCameraReader::timeOut : can not convert mat to QImage.";
        return ;
    }

//    qDebug() << "emit updateCameraDisp().";
    emit updateCameraDisp(image);
}

QImage CCameraReader::Mat2QImage(const Mat& frame)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(frame.type() == CV_8UC1)
    {
        QImage image(frame.cols, frame.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = frame.data;
        for(int row = 0; row < frame.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, frame.cols);
            pSrc += frame.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(frame.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)frame.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(frame.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)frame.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, frame.cols, frame.rows, frame.step, QImage::Format_ARGB32);
        return image;
    }
    else
        throw QString("ERROR: Mat could not be converted to QImage.");
}
