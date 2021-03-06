#ifndef CPREDICTOR_H
#define CPREDICTOR_H

#include <QList>
#include "cv.h"
#include <ml.h>
#include <QObject>
#include <QThread>

using namespace cv;
using namespace cv::ml;

#include "cdatabase.h"
#include "cserialreader.h"
namespace Sit{
    enum SitType {NORMAL=0, BACKWARD, FORWARD, RIGHTWARD, LEFTWARD, UNKNOWN};
}

///
/// \brief The CPredictor class provide method to collect, train, store and predict.
///
class CPredictor : public QObject
{
    Q_OBJECT
public:
    Ptr<RTrees> mp_trees;
    static const int TYPE_NUM = 6;

private:
    Mat  iData;
    Mat  iLabel;
    CDatabase* mp_db;

public:
    CPredictor();
    ~CPredictor();

    static QString Enum2String(Sit::SitType sitType);

    Sit::SitType classify(const QList<int> & iPredictData);
    void loadFromDB();
    void save2DB(bool b_exist);
    void train();
    bool isTrained();
    void collectCertainType(const CSerialReader&, Sit::SitType type);
    void storeTrainData(Sit::SitType type,const QList<QList<int>>& data);

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
