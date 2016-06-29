#include "ccamerareader.h"
#include "videoio.hpp"
#include "imgproc.hpp"
#include <QString>

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
    mp_camera->release();
}
