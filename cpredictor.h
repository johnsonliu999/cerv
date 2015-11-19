#ifndef CPREDICTOR_H
#define CPREDICTOR_H

#include <QList>
#include <cv.h>
#include <ml.h>

using namespace cv;
using namespace cv::ml;

/**
    说明： 单例模式
    使用方法：
        先获取单例，调用isTrained（）判断可用性。
        若未训练，则需collectData（），data需要五个坐姿的数据，然后train（），就可以用了-------------可以考虑存盘 save2DB().
        若已训练，直接用
     唯一有用的接口： predict（）

*/
class CPredictor
{
public:
    enum eSitType {NORMAL=0, BACKWARD=6, FORWARD=7, RIGHTWARD=12, LEFTWARD=13};

public:
    static CPredictor* getPredictor();
    eSitType Predict(const QList<int> & iPredictData);
    void loadFromDB();
    void save2DB();
    void train();
    bool isTrained();
    void beginReceiveData();

    void CollectData(eSitType type);

private:   
    Ptr<RTrees> pTrees;
    Mat  iData;
    Mat  iLabel;

    static CPredictor* m_pPredictor;
    CPredictor();



private:

    void __BuildRTrees(Mat & i_Data, Mat & i_Label);


};

#endif // CPREDICTOR_H
