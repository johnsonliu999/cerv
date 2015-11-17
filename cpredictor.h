#ifndef CPREDICTOR_H
#define CPREDICTOR_H

#include <QString>
#include <QSqlError>
#include <QSqlDatabase>
#include <QList>
#include <cv.h>
#include <ml.h>

using namespace cv;
using namespace cv::ml;

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

private:   
    Ptr<RTrees> pTrees;

    static CPredictor* m_pPredictor;
    CPredictor();



private:
    bool __PrepareData(const QString &cUserName, Mat &iData, Mat &iLabel, QSqlDatabase iConnection);
    void __BuildRTrees(Mat & i_Data, Mat & i_Label);


};

#endif // CPREDICTOR_H
