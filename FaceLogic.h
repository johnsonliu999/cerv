#pragma once
#ifndef FACELOGIC_H
#define FACELOGIC_H

#include "cfaceclassfier.h"
#include "ccamerareader.h"

#include <QList>
#include <QString>
#include <QObject>

#include "cv.h"
using namespace cv;

///
/// \brief The FaceLogic class
/// process face related logic.
///
class FaceLogic : public QObject
{
    Q_OBJECT
public:
    FaceLogic();
    ~FaceLogic();

    static QString Enum2String(const Face::FaceType &faceType);

    void start();
    void stop();

//    static bool storeNormalValue(const Point&,const Point&,const Point&);
//    static QList<Point> getNormalValue();
//    static void judgeFacePosture(const Point&,const Point&,const Point&);
//	static bool isInitialized();

private:
    Face::FaceType getRecentRes();

signals:
    void updateDisp(QString text);
    void info(QString title, QString text);

public slots:
    void updateFaceRes();
    void getRecentType(Face::FaceType &type);

private:
    QList<Face::FaceType> *mp_resList;
    QList<int> *mp_statList;
    CFaceClassfier *mp_classifier;
    CCameraReader *mp_reader;
};

#endif //FACELOGIC_H
