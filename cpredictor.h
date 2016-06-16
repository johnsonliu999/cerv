#ifndef CPREDICTOR_H
#define CPREDICTOR_H

#include <QList>
#include "cv.h"
#include <ml.h>
#include <QObject>
#include <QThread>

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

#include "cdatabase.h"

///
/// \brief The CPredictor class provide method to collect, train, store and predict.
///
class CPredictor : public QObject
{
    Q_OBJECT
public:
    Ptr<RTrees> mp_trees;

    enum eSitType {NORMAL=0, BACKWARD, FORWARD, RIGHTWARD, LEFTWARD, UNKNOWN};
    static int nSitTypeNumber;

private:
    Mat  iData;
    Mat  iLabel;
    CDatabase m_db;

    static CPredictor* mp_predictor;



public:
    static CPredictor* getPredictor();
    static const QString getSitString(int index);

    eSitType Predict(const QList<int> & iPredictData);
    void loadFromDB(const User &user);
    void save2DB(bool r_replace);
    void train();
    bool isTrained();
    void collectCertainType(eSitType type);
    void CollectDataRaw(eSitType type,const QList<QList<int>>& data);

public slots:
    void trainData(QString portName, const bool b_replace);
    void tryLoadModel();

protected:
    void __BuildRTrees(Mat & i_Data, Mat & i_Label);


private:
    CPredictor();
    ~CPredictor();

signals:
    void percentChanged(int percent);
    void information(const QString title, const QString content);
    void critial(const QString title, const QString content);
    void finishedTrain();
};

#endif // CPREDICTOR_H
