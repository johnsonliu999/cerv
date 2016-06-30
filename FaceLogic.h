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

private:
    Face::FaceType getRecentRes();

signals:
    void updateDisp(QString text);
    void updateFaceDisp(const QImage& img);
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
