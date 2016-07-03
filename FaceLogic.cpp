#include "FaceLogic.h"
#include <fstream>
#include <math.h>
#include <QDebug>
#include <string>

#include "cv.h"
#include "imgproc.hpp"
using namespace cv;

const int RES_NUM = 5; ///< the number of results kept

///
/// \brief FaceLogic::getRecentRes
/// finds the most common type in result list.
///
/// \return
///
Face::FaceType FaceLogic::getRecentRes()
{
    int ind = 0;
    for (int i = 0; i < CFaceClassfier::TYPE_NUM; i++)
    {
        if ((*mp_statList)[ind] < (*mp_statList)[i])
            ind = i;
    }

    return Face::FaceType(ind);
}


///
/// \brief FaceLogic::storeNormalValue store position of eye and mouth.
/// \param lfeye Left eye position.
/// \param rteye Right eye position.
/// \param mouth Mouth position.
/// \return success or not.
///
FaceLogic::FaceLogic(QObject *parent) :
    QObject(parent),
    mp_thread(new QThread(this)),
    mp_resList(new QList<Face::FaceType>),
    mp_statList(new QList<int>),
    mp_reader(new CCameraReader),
    mp_classifier(new CFaceClassfier)
{
    for (int i = 0; i < CFaceClassfier::TYPE_NUM; i++)
        *mp_statList << 0;
}

FaceLogic::~FaceLogic()
{

}

///
/// \brief FaceLogic::start
/// starts face logic process.
/// Including resetting all records and reloading models.
///
void FaceLogic::start()
{
    for (int i = 0; i < RES_NUM; i++)
        (*mp_statList)[i] = 0;

    mp_resList->clear();

    try{
        mp_reader->openCamera();
    } catch(const QString &e)
    {
        qDebug() << "Open camera failed.";
        throw e;
    }

    try{
        mp_classifier->loadModel("./train_file/");
        mp_classifier->loadFromDB();
    } catch(const QString& e)
    {
        mp_reader->closeCamera();
        qDebug() << "FaceLogic::start : Failed to load face classifying model.";
        throw e;
    }
}

///
/// \brief FaceLogic::stop
/// stops face logic process.
///
void FaceLogic::stop()
{
    mp_reader->closeCamera();
}


// begin slots

#include <QTime>
///
/// \brief FaceLogic::updateFaceRes
/// update recent result of classification.
/// Wait for 'update' to use.
/// Current update 5 results each time.
/// 3500 msecs each time.
///
void FaceLogic::updateFaceRes()
{
//    QTime t1 = QTime::currentTime();
    Mat frame;
    QPoint center;
    Face::FaceType res;
    QList<QPoint> points;
    for (int i = 0; i < RES_NUM; i++)
    {
        frame = mp_reader->getFrame();
        res = mp_classifier->clarrify(frame, points);

        for (int i = 0; i < points.size(); i++)
            drawMarker(frame, Point(points[i].x(), points[i].y()), Scalar(255,255,255));

        emit updateFaceDisp(CCameraReader::Mat2QImage(frame));

        // update statistics
        if (mp_resList->size() >= RES_NUM)
        {
            (*mp_statList)[(int)(*mp_resList)[0]]--;
            mp_resList->removeFirst();
        }
        mp_resList->append(res);
        (*mp_statList)[(int)res]++;
    }
//    QTime t2 = QTime::currentTime();
//    qDebug() << "Face time cost :" << t1.msecsTo(t2);

    emit updateDisp(CFaceClassfier::Enum2String(getRecentRes()));
}

void FaceLogic::getRecentType(Face::FaceType &type)
{
    type = getRecentRes();
}


