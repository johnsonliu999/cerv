#ifndef CPREDICTOR_H
#define CPREDICTOR_H

#include <QList>
#include <cv.h>
#include <ml.h>
#include <QObject>

#include "User.h"

using namespace cv;
using namespace cv::ml;

/**
    说明： 单例模式
    使用方法：
        先获取单例，调用isTrained（）判断可用性。
        若未训练，则需collectData（），data需要五个坐姿的数据，然后train（），就可以用了-------------可以考虑存盘 save2DB().
        或者 loadFromDb(),看是否有存盘的备份
        若已训练，直接用
     唯一有用的接口： predict（）

*/
class CPredictor:public QObject
{
    Q_OBJECT

public:
    enum eSitType {NORMAL=0, BACKWARD, FORWARD, RIGHTWARD, LEFTWARD};
    static int nSitTypeNumber;

public:
    static CPredictor* getPredictor();
    static const QString getSitString(int index);

    eSitType Predict(const QList<int> & iPredictData);
    void loadFromDB(const User &user);
    void save2DB();
    void train();
    bool isTrained();
    // void beginReceiveData();

    void CollectDataFromSerialPort(eSitType type);
    void CollectDataRaw(eSitType type,const QList<QList<int>>& data);
private:   
    Ptr<RTrees> pTrees;
    Mat  iData;
    Mat  iLabel;

    static CPredictor* m_pPredictor;
    CPredictor();
    ~CPredictor();

signals:
    void percentChanged(int percent);

protected:

    void __BuildRTrees(Mat & i_Data, Mat & i_Label);


};

#endif // CPREDICTOR_H
