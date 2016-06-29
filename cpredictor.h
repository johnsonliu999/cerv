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
    enum SitType {NORMAL=0, BACKWARD, FORWARD, RIGHTWARD, LEFTWARD, UNKNOWN};

private:
    Mat  iData;
    Mat  iLabel;
    CDatabase* mp_db;

public:
    CPredictor();
    ~CPredictor();

    SitType classify(const QList<int> & iPredictData);
    void loadFromDB();
    void save2DB(bool b_exist);
    void train();
    bool isTrained();
    void collectCertainType(const CSerialReader&, SitType type);
    void storeTrainData(SitType type,const QList<QList<int>>& data);

public slots:
    void trainData(QString portName, const bool b_replace);

protected:
    void __BuildRTrees(Mat & i_Data, Mat & i_Label);


signals:
    void percentChanged(int percent);
    void information(const QString title, const QString content);
    void finishedTrain();
};

#endif // CPREDICTOR_H
