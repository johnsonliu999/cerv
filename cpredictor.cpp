#include "cpredictor.h"
#include <QDebug>
#include <QThread>


//dao model  session
#include "Predictor.h"
#include "DAO.h"
#include "MySession.h"

const int ATTRIBUTE_PRE_SAMPLE = 20;
const int NUMBER_OF_TRAINING_SAMPLE_PER_CLASS = 500;

CPredictor* CPredictor::mp_predictor = NULL;
int CPredictor::nSitTypeNumber = 5;


#include "cdatabase.h"
/**
   在构造的时候准备好两个成员变量  iData, iLabel
*/
CPredictor::CPredictor():iData(NUMBER_OF_TRAINING_SAMPLE_PER_CLASS * 5, ATTRIBUTE_PRE_SAMPLE, CV_32FC1),
    iLabel(NUMBER_OF_TRAINING_SAMPLE_PER_CLASS * 5, 1, CV_32SC1)
{
    DBParams params;
    params.database = "QMYSQL";
    params.host = "localhost";
    params.database = "neck";
    params.user = "root";
    params.password = "qq452977491";
    params.port = 3306;

    m_db = CDatabase("predictor", params);
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

    mp_trees->predict(iPredictData, iPredictLabel);
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
    if(!mp_predictor)
        mp_predictor =new CPredictor();
    return mp_predictor;
}

///
/// \brief CPredictor::getSitString return string format infomation.
/// \param index Type of seat.
/// \return Seat type in string format.
///
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
void CPredictor::collectCertainType(const CSerialReader& reader,eSitType type)
{
    QList< QList<int> > iDataListList;

    // 每个类型的记录条数，用来判断是否达标
    int nTotalNumber = 0;
    try{
        qDebug() << "Now collect" <<" SitType ["  <<SitLogic::fetchJudgedMessage(type)<<"] "<< "data, please keep";
        // since the matrix start with 0
        while (nTotalNumber + 1 < NUMBER_OF_TRAINING_SAMPLE_PER_CLASS)
        {
            iDataListList = reader.readSerial();

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
    } catch (const QString& e)
    {
        emit information("collectCertainType", e);
    }

    qDebug() << "finished collect" <<" SitType ["  <<SitLogic::fetchJudgedMessage(type)<<"] "<< "data, congratulation";

}

///
/// \brief CPredictor::CollectDataRaw store data.
/// \param[in] type Type of seat.
/// \param[in] data Data read from serial port.
///
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
    try{
        QSqlDatabase db = m_db.getDB();
        Predictor p =DAO::query(db, user);
        if(p.id!=0)
        mp_trees=mp_trees->loadFromString<RTrees>((String)p.xml.toStdString());
        db.close();
    } catch (const QString& e)
    {
        emit information("loadFromDB", e);
    }
}


#include <QFile>
void CPredictor::save2DB(bool b_replace)
{

    mp_trees->save("temp.xml");

    //read file to database,and delete the temp file.
    Predictor p;
    p.user = Session::user;
    QFile f("temp.xml");
    f.open(QIODevice::ReadOnly);
    p.xml = f.readAll();
    try{
        QSqlDatabase db = m_db.getDB();
        if (b_replace) DAO::update(db, p);
        else DAO::insert(db, p);
        db.close();
    } catch(const QString& e)
    {
        //f.remove();
        emit information("Save2DB", e);
    }

    //f.remove();
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
        QSerialPort* p_port = reader.getPort();

        for (int i = 0; i < nSitTypeNumber; i++)
        {
            emit information("Collect information", "Going to collect " + getSitString(i) + " data");
            collectCertainType(reader, (eSitType)i);
        }

        p_port->close();
        emit information("Collect information", "Finished collect");

    }catch (const QString & e)
    {
        emit critial("Collect error", e);
        return;
    }

    try{
        train();
        emit information("Train information", "Finished train");
    }catch (const QString & e)
    {
        emit critial("Train error", e);
        return;
    }

    save2DB(b_replace);
    emit finishedTrain();

}

void CPredictor::tryLoadModel()
{
    try{
        QSqlDatabase db = m_db.getDB();
        Predictor p = DAO::query(db, Session::user);
        db.close();
        if (p.id != 0)
        {
            loadFromDB(Session::user);
            emit information("Train information", "Find model");
        }
        else
        {
            emit information("Train Information", "No model available");
            return;
        }
    }catch (const QString & e)
    {
        emit information("Train error", e);
    }
}
