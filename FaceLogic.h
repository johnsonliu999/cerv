#pragma once
#ifndef FACELOGIC_H
#define FACELOGIC_H


#include "core/core.hpp"


#include <QList>
#include <QString>
#include <QObject>

using namespace cv;


#include "cfaceclassfier.h"
#include "ccamerareader.h"

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

    static QString Enum2String(const CFaceClassfier::FaceType &faceType);

    void start();
    void stop();

//    static bool storeNormalValue(const Point&,const Point&,const Point&);

//    static QList<Point> getNormalValue();


//    static void judgeFacePosture(const Point&,const Point&,const Point&);


//	static bool isInitialized();

private:
    CFaceClassfier::FaceType getRecentRes();

signals:
    void updateDisp(QString text);
    void info(QString title, QString text);

public slots:
    void updateFaceRes();

private:
    QList<CFaceClassfier::FaceType> *mp_resList;
    QList<int> *mp_statList;
    CFaceClassfier *mp_classifier;
    CCameraReader *mp_reader;
};

#endif //FACELOGIC_H
