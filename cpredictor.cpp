#include "cpredictor.h"
#include <QDebug>
#include <QThread>


//dao model  session
#include "Predictor.h"
#include "DAO.h"
#include "MySession.h"

#include "moc_cpredictor.cpp"

#define ATTRIBUTE_PRE_SAMPLE 20
#define NUMBER_OF_TRAINING_SAMPLE_PER_CLASS 100

CPredictor* CPredictor::m_pPredictor = NULL;
int CPredictor::nSitTypeNumber = 5;



/**
   在构造的时候准备好两个成员变量  iData, iLabel
*/
CPredictor::CPredictor():iData(NUMBER_OF_TRAINING_SAMPLE_PER_CLASS * 5, ATTRIBUTE_PRE_SAMPLE, CV_32FC1),
    iLabel(NUMBER_OF_TRAINING_SAMPLE_PER_CLASS * 5, 1, CV_32SC1)
{

}

CPredictor::~CPredictor()
{

}

/**
 * @brief CPredictor::Predict
 * 预测函数，需在Initialize后调用
 *
 * @param pnPredictData：由各个特征组成的数组，NUMBER_OF_TRAINING_SAMPLE_PER_CLASS项
 * @return ：坐姿的枚举类型
 */
CPredictor::eSitType CPredictor::Predict(const QList<int> & iPredictDataList)
{
    Mat iPredictData = Mat(1, ATTRIBUTE_PRE_SAMPLE, CV_32FC1);
    Mat iPredictLabel = Mat(1,1, CV_32SC1);
    for (int i = 0; i < ATTRIBUTE_PRE_SAMPLE; i++)
        iPredictData.at<float>(0,i) = iPredictDataList[i];

    pTrees->predict(iPredictData, iPredictLabel);
    int nPredictLable = static_cast<int>(iPredictLabel.at<float>(0,0));

    qDebug() << "Finished predict";
    qDebug() << "The result is" << nPredictLable;

    return static_cast<eSitType>(nPredictLable);
}

/**
 * @brief CPredictor::getPredictor
 * if the user has been trained before will return last
 * model directly
 *
 *
 * @param cUserName
 * @return ：成功返回实例指针，失败返回NULL
 */
CPredictor *CPredictor::getPredictor()
{
    if(m_pPredictor ==NULL)
        m_pPredictor =new CPredictor();
    return m_pPredictor;
}

const QString CPredictor::getSitString(int index)
{
    switch ((CPredictor::eSitType)index)
    {
    case CPredictor::NORMAL:
        return "normal";
    case CPredictor::BACKWARD:
        return "backward";
    case CPredictor::FORWARD:
        return "forward";
    case CPredictor::RIGHTWARD:
        return "rightward";
    case CPredictor::LEFTWARD:
        return "leftward";
    }
}


/**
 * @brief CPredictor::__BuildRTrees
 * 创建随机森林模型
 * 同时将模型进行存储，名字为用户名cUserName
 *
 * @param iData：数据集
 * @param iLabel：分类标签
 * @return ：false表示失败，成功返回true
 */
void CPredictor::__BuildRTrees(Mat &iData, Mat &iLabel)
{
    // 创建训练数据集及随机森林模型
    Ptr<TrainData> pTrainData = TrainData::create(iData, ROW_SAMPLE, iLabel);
    pTrees = RTrees::create();

    // 设置训练参数
    pTrees->setMaxDepth(10);
    pTrees->setMinSampleCount(10);
    pTrees->setRegressionAccuracy(0);
    pTrees->setUseSurrogates(false);
    pTrees->setMaxCategories(5);
    pTrees->setPriors(Mat());
    pTrees->setActiveVarCount(20);
    pTrees->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER+TermCriteria::EPS, 100, 0.01f));

    if (pTrees->train(pTrainData))
    {
        qDebug() << "Train succeed";      
    }
    else
    {
        throw QString("Train failed");
    }
}

/**
 * @brief CPredictor::CollectData
 * collect original data, suppose the device already
 * connected and correct can be read through serial
 * port
 *
 * @return
 */

#include "cserialreader.h"
#include "SitLogic.h"

/**
 * @brief CPredictor::CollectDataFromSerialPort
 * collect a specific sit type with number of NUMBER_OF_TRAINING_SAMPLE_PER_CLASS
 * since the matrix start with 0, so we can see many details of marginal 1
 * be careful with the bound
 *
 * we use the type as int to set the start offset of the storage
 *
 * @param type
 */
void CPredictor::collectCertainType(eSitType type)
{

    CSerialReader* pReader = CSerialReader::getReader();
    pReader->OpenSerial(ARDUINO_ID);
    pReader->ConnectDevice();

    QList< QList<int> > iDataListList;

    // 每个类型的记录条数，用来判断是否达标
    int nTotalNumber = 0;

    qDebug() << "Now collect" <<" SitType ["  <<SitLogic::fetchJudgedMessage(type)<<"] "<< "data, please keep";
    // since the matrix start with 0
    while (nTotalNumber + 1 < NUMBER_OF_TRAINING_SAMPLE_PER_CLASS)
    {
        iDataListList = pReader->ReadSerial();

        int size = iDataListList.size();
        if (nTotalNumber + size >= NUMBER_OF_TRAINING_SAMPLE_PER_CLASS)
            size = NUMBER_OF_TRAINING_SAMPLE_PER_CLASS - nTotalNumber - 1;

        for (int j = 0; j < size; j++)
        {
            for (int k = 0; k < ATTRIBUTE_PRE_SAMPLE; k++)
            {
                iData.at<float>((int)type * NUMBER_OF_TRAINING_SAMPLE_PER_CLASS + j + nTotalNumber, k) = static_cast<float>(iDataListList[j][k]);
            }
            iLabel.at<int>((int)type * NUMBER_OF_TRAINING_SAMPLE_PER_CLASS + j + nTotalNumber, 0) = static_cast<int>(type);
        }
        nTotalNumber += size;
        qDebug() << "collected" << nTotalNumber;
        emit percentChanged((int)(((float)nTotalNumber + 1)/NUMBER_OF_TRAINING_SAMPLE_PER_CLASS * 100));
        QThread::sleep(5);
    }
    qDebug() << "finished collect" <<" SitType ["  <<SitLogic::fetchJudgedMessage(type)<<"] "<< "data, congratulation";

    pReader->CloseSerial();

}

void CPredictor::CollectDataRaw(CPredictor::eSitType type, const QList<QList<int> > & data)
{

    for (int j = 0; j < data.size(); j++)
    {
        for (int k = 0; k < ATTRIBUTE_PRE_SAMPLE; k++)
        {
            iData.at<float>(j, k) = static_cast<float>(data[j][k]);
        }
        iLabel.at<int>(j, 0) = static_cast<int>(type);
    }

}

void CPredictor::loadFromDB(const User& user)
{

     Predictor p =DAO::query(user);
     if(p.id!=0)
        pTrees=pTrees->loadFromString<RTrees>((String)p.xml.toStdString());
}


#include <QFile>
void CPredictor::save2DB()
{
     pTrees->save("temp.xml");

     //read file to database,and delete the temp file.

     Predictor p;
     p.user =Session::user;
     QFile f("temp.xml");
     f.open(QIODevice::ReadOnly);
     p.xml  =f.readAll();

     DAO::insert(p);



     f.remove();
}

void CPredictor::train()
{
    __BuildRTrees(iData, iLabel);

}

bool CPredictor::isTrained()
{
    return   pTrees->isTrained();
}

void CPredictor::trainData()
{


    try{
        for (int i = 0; i < nSitTypeNumber; i++)
        {
            emit information("Collect information", "Going to collect " + getSitString(i) + " data");
            collectCertainType((eSitType)i);
        }
        emit information("Collect information", "Finished collect");

    }catch (const QString & e)
    {
        emit critial("Collect error", e);
    }

    try{
        train();
        emit information("Train information", "Finished train");
    }catch (const QString & e)
    {
        emit critial("Train error", e);
    }

    save2DB();
}

void CPredictor::tryLoadModel()
{
    try{
        Predictor p = DAO::query(Session::user);
        if (p.id != 0)
        {
            loadFromDB(Session::user);
            emit information("Train information", "Find model");
        }
        else
        {
            emit information("Train Information", "No model available");
            trainData();
        }
    }catch (const QString & e)
    {
        emit critial("Train error", e);
    }
}
