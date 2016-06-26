#ifndef TOTALLOGIC_H
#define TOTALLOGIC_H



#include "FaceLogic.h"
#include "SitLogic.h"


enum  totalType{};

///
/// \brief The TotalLogic class control total logic.
///

class TotalLogic
{
public:
    TotalLogic();


    static totalType judge()
    {
        FacePostureType t1 =FaceLogic::getRtType();
        SitLogic::readOne();
        eSitType t2 =SitLogic::getRecentRes();

        //侧脸等，此时只有坐垫有效
        if(t1>=1&&t1<=3)
        {
            return t2;
        }
        //上下脸，此时只有人脸识别有效
        else if(t1==8||t1==9)
        {
            return t1;
        }
        //其他的，综合判断
        else
        {

        }

    }

private:
    float weight =0.7;

};

#endif // TOTALLOGIC_H
