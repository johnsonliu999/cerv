#pragma once
#ifndef FACELOGIC_H
#define FACELOGIC_H


#include "opencv2/core/core.hpp"
#include <QList>
#include <QString>
using namespace cv;

enum FacePostureType{ Normal, Profile,NO_FACE,Fault_FACE, Near, Far, Left, Right, Up, Down};

class FaceLogic
{
public:

    static bool storeNormalValue(const Point&,const Point&,const Point&);

    static QList<Point> getNormalValue();


    static void judgeFacePosture(const Point&,const Point&,const Point&);

    static QString fetchJudgedMessage(FacePostureType faceType=rtType);


    static FacePostureType getRtType();
    static void setRtType(FacePostureType rtType);

private:

    static FacePostureType rtType;
    static QList<Point> normal;

public:
	static bool isInitialized();
};

#endif //FACELOGIC_H
