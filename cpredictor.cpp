#include "cpredictor.h"
#include <QDebug>
#include <QThread>


//dao model  session
#include "Predictor.h"
#include "DAO.h"
#include "MySession.h"

const int ATTRIBUTE_PRE_SAMPLE = 20;
const int NUMBER_OF_TRAINING_SAMPLE_PER_CLASS = 500;

const int SIT_TYPE_NUM = 6;

#include "cdatabase.h"

/// CPredictor is mainly used for classfying data.
/// * CPredictor keeps a pointer of model used for classfication.\n
/// * Include [save2DB] [loadFromDB], which refers to DB,
/// so the object should keep a connection or object of DB.
/// The connection name is 'predictor'\n
/// * All DB operation(opening, closing) should be limited in the class.\n

///
/// \brief CPredictor::CPredictor
/// construct CPredictor and connect DB.
///
CPredictor::CPredictor():
    iData(NUMBER_OF_TRAINING_SAMPLE_PER_CLASS * 5, ATTRIBUTE_PRE_SAMPLE, CV_32FC1),
    iLabel(NUMBER_OF_TRAINING_SAMPLE_PER_CLASS * 5, 1, CV_32SC1),
    mp_db(new CDatabase("predictor",
            DBParams("QMYSQL", "localhost", "neck", "root", "qq452977491", 3306)))
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
Sit::SitType CPredictor::classify(const QList<int> & iPredictDataList)
{
    Mat iPredictData = Mat(1, ATTRIBUTE_PRE_SAMPLE, CV_32FC1);
    Mat iPredictLabel = Mat(1,1, CV_32SC1);
    for (int i = 0; i < ATTRIBUTE_PRE_SAMPLE; i++)
        iPredictData.at<float>(0,i) = iPredictDataList[i];

    mp_trees->predict(iPredictData, iPredictLabel);
    int nPredictLable = static_cast<int>(iPredictLabel.at<float>(0,0));

//    qDebug() << "Finished predict";
//    qDebug() << "The result is" << nPredictLable;

    return static_cast<Sit::SitType>(nPredictLable);
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
    mp_trees = RTrees::create();

    // 设置训练参数
    mp_trees->setMaxDepth(10);
    mp_trees->setMinSampleCount(10);
    mp_trees->setRegressionAccuracy(0);
    mp_trees->setUseSurrogates(false);
    mp_trees->setMaxCategories(5);
    mp_trees->setPriors(Mat());
    mp_trees->setActiveVarCount(20);
    mp_trees->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER+TermCriteria::EPS, 100, 0.01f));

    if (mp_trees->train(pTrainData))
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
 * collect a specific seat type with number of NUMBER_OF_TRAINING_SAMPLE_PER_CLASS
 * since the matrix start with 0, so we can see many details of marginal 1
 * be careful with the bound
 *
 * we use the type as int to set the start offset of the storage
 *
 * @param type
 */
void CPredictor::collectCertainType(const CSerialReader& reader,Sit::SitType type)
{
    QList< QList<int> > recList; // 每个类型的记录条数，用来判断是否达标
    int total = 0;

    qDebug() << "Now collect" <<" Sit::SitType ["  <<SitLogic::Enum2String(type)<<"] "<< "data, please keep";
    // since the matrix start with 0
    while (total + 1 < NUMBER_OF_TRAINING_SAMPLE_PER_CLASS)
    {
        recList = reader.getTrainData();

        int size = recList.size();
        if (total + size >= NUMBER_OF_TRAINING_SAMPLE_PER_CLASS)
            size = NUMBER_OF_TRAINING_SAMPLE_PER_CLASS - total - 1;

        for (int j = 0; j < size; j++)
        {
            for (int k = 0; k < ATTRIBUTE_PRE_SAMPLE; k++)
            {
                iData.at<float>((int)type * NUMBER_OF_TRAINING_SAMPLE_PER_CLASS + j + total, k) = static_cast<float>(recList[j][k]);
            }
            iLabel.at<int>((int)type * NUMBER_OF_TRAINING_SAMPLE_PER_CLASS + j + total, 0) = static_cast<int>(type);
        }
        total += size;
        qDebug() << "collected" << total;
        emit percentChanged((int)(((float)total + 1)/NUMBER_OF_TRAINING_SAMPLE_PER_CLASS * 100));
        QThread::sleep(5);
    }

    qDebug() << "finished collect" <<" Sit::SitType ["  <<SitLogic::Enum2String(type)<<"] "<< "data, congratulation";

}

///
/// \brief CPredictor::storeTrainData
/// store data for training.
///
/// \param type
/// \param data
///
void CPredictor::storeTrainData(Sit::SitType type, const QList<QList<int> > & data)
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

///
/// \brief CPredictor::loadFromDB
/// load model from DB according to User structure.
///
/// \exception <QString> {}
///
/// \param user
///
void CPredictor::loadFromDB()
{
    try {
        mp_db->openDB();
    } catch(const QString &e)
    {
        qDebug() << "CPredictor::loadFromDB:Can't open DB";
        throw e;
    }
    QByteArray xml;
    try{
        xml = mp_db->selectXml();
    } catch (const QString& e)
    {
        mp_db->closeDB();
        qDebug() << "CPredictor::loadFromDB():Cannot select xml";
        throw e;
    }

    mp_trees=mp_trees->loadFromString<RTrees>((String)xml.toStdString());
    mp_db->closeDB();
}


#include <QFile>
void CPredictor::save2DB(bool b_exist)
{
    try{
        mp_db->openDB();
    } catch(const QString &e)
    {
        qDebug() << "save2DB:" << e;
        throw e;
    }

    mp_trees->save("temp.xml");
    //read file to database,and delete the temp file.
    QFile f("temp.xml");
    f.open(QIODevice::ReadOnly);
    QByteArray xml = f.readAll();
    try{
        if(b_exist) mp_db->updateXml(xml);
        else mp_db->insertXml(xml);
    } catch(const QString& e)
    {
        f.remove();
        mp_db->closeDB();
        emit information("Save2DB", e);
    }

    mp_db->closeDB();
    f.remove();
}

void CPredictor::train()
{
    __BuildRTrees(iData, iLabel);

}

bool CPredictor::isTrained()
{
    return   mp_trees->isTrained();
}

void CPredictor::trainData(const QString portName, const bool b_replace)
{
    try{
        CSerialReader reader(portName);

        for (int i = 0; i < SIT_TYPE_NUM; i++)
        {
            emit information("Collect information", "Going to collect " + SitLogic::Enum2String((Sit::SitType)i) + " data");
            collectCertainType(reader, (Sit::SitType)i);
        }

        emit information("Collect information", "Finished collect");

    }catch (const QString & e)
    {
        emit information("trainData", e);
        return;
    }

    try{
        train();
        emit information("Train information", "Finished train");
    }catch (const QString & e)
    {
        emit information("Train error", e);
        return;
    }

    save2DB(b_replace);
    emit finishedTrain();

}
