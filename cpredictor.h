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

#include "cdatabase.h"

class CSerialReader;

///
/// \brief The CPredictor class provide method to collect, train, store and predict.
///
class CPredictor : public QObject
{
    Q_OBJECT
public:
    Ptr<RTrees> mp_trees;
    enum eSitType {NORMAL=0, BACKWARD, FORWARD, RIGHTWARD, LEFTWARD, UNKNOWN};

private:
    Mat  iData;
    Mat  iLabel;
    CDatabase m_db;

public:
    CPredictor();
    ~CPredictor();

    eSitType predict(const QList<int> & iPredictData);
    void loadFromDB(const User &user);
    void save2DB(bool r_replace);
    void train();
    bool isTrained();
    void collectCertainType(const CSerialReader&, eSitType type);
    void CollectDataRaw(eSitType type,const QList<QList<int>>& data);

public slots:
    void trainData(QString portName, const bool b_replace);

protected:
    void __BuildRTrees(Mat & i_Data, Mat & i_Label);


signals:
    void percentChanged(int percent);
    void information(const QString title, const QString content);
    void critial(const QString title, const QString content);
    void finishedTrain();
};

#endif // CPREDICTOR_H
