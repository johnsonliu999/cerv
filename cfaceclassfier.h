#ifndef CFACECLASSFIER_H
#define CFACECLASSFIER_H

#include <vector>

#include "cv.h"
#include "objdetect.hpp"
using namespace cv;

#include <QObject>
#include <QPoint>

#include "ccamerareader.h"
#include "cdatabase.h"

namespace Face {
    enum FaceType {NORMAL=0, BACKWARD, FORWARD, RIGHTWARD, LEFTWARD, UNKNOWN};
}

struct OrgansCoordinate
{
    QPoint mouth;
    QPoint leftEye;
    QPoint rightEye;

    OrgansCoordinate() {}
    OrgansCoordinate(const QPoint &mouth, const QPoint &leftEye, const QPoint &rightEye) :
        mouth(mouth),leftEye(leftEye),rightEye(rightEye) {}
};

///
/// \brief CFaceClassfier::CFaceClassfier
/// is used to clasify face type.
///
/// CFaceClassfier uses data from CCameraReader as sample data,
/// load classfier model when constructed.
/// Final face type will be store in FaceLogic.
///
///
class CFaceClassfier : public QObject
{
    Q_OBJECT
public:
    static const int TYPE_NUM = 6;
public:
    CFaceClassfier();
    ~CFaceClassfier();

    Face::FaceType clarrify(const Mat &frame, QList<QPoint> &points);

    void loadModel(const cv::String &parentPath);
    void loadFromDB();
    void save2DB();
    void save2DB(bool b_exist);

private:
    cv::Rect getMaxRect(const std::vector<cv::Rect> &rects);
    int calcDist(const QPoint &p1, const QPoint &p2);
    cv::Mat framePreproc(const cv::Mat frame);
    Face::FaceType clarrifyProfile(const cv::Mat grayFrame, QList<QPoint> &points);
    Face::FaceType clarrifyFace(const cv::Mat grayFrame, QList<QPoint> &points);
    QPoint calcAverage(const QList<QPoint> &pointList);

//    Face::FaceType clarrifyByOrgans(const QPoint &leftEye, const QPoint &rightEye, const QPoint &mouth);

private:
    CascadeClassifier* mp_faceClassfier;
    CascadeClassifier* mp_eyesClassfier;
    CascadeClassifier* mp_mouthClassfier;
    CascadeClassifier* mp_profileClassfier;

    OrgansCoordinate* mp_coordinate;
    CDatabase *mp_db;

signals:
    void info(const QString title, const QString text);
    void updateDisp(const QImage& img, const QString &remainTime);

public slots:
    void train();
};

#endif // CFACECLASSFIER_H
