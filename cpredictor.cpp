#include "cpredictor.h"
#include <QDebug>


//dao model  session
#include "Predictor.h"
#include "DAO.h"
#include "MySession.h"


#define ATTRIBUTE_PRE_SAMPLE 20
#define NUMBER_OF_TRAINING_SAMPLE_PER_CLASS 1000

CPredictor* CPredictor::m_pPredictor = NULL;



/**
   在构造的时候准备好两个成员变量  iData, iLabel
*/
CPredictor::CPredictor():iData(NUMBER_OF_TRAINING_SAMPLE_PER_CLASS * 5, ATTRIBUTE_PRE_SAMPLE, CV_32FC1),
    iLabel(NUMBER_OF_TRAINING_SAMPLE_PER_CLASS * 5, 1, CV_32SC1)
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

/*/**
 * @brief CPredictor::__PrepareData
 * 准备好训练数据
 *
 * @param cUserName：用户的名字即表名
 * @param iData：用来装训练数据的矩阵
 * @param iLabel：用来装分类的矩阵，1列
 * @param iConnection：数据库连接实体
 * @return：true表示成功，false表示失败
 **/
//bool CPredictor::__PrepareData(const QString & cUserName, Mat & iData, Mat & iLabel, QSqlDatabase iConnection)
//{
//    // 构建查询语句
//    QString cSql = "select * from " + cUserName + " where kind = :id order by dataid limit " + QString::number(NUMBER_OF_TRAINING_SAMPLE_PER_CLASS);

//    int gnClass[5] = {NORMAL, BACKWARD, FORWARD, RIGHTWARD, LEFTWARD};

//    // 准备查询接口
//    QSqlQuery iQuery(iConnection);
//    if (false == iQuery.prepare(cSql))
//    {
//        qDebug() << "Query prepare failed";
//        qDebug() << iQuery.lastError().text();
//        return false;
//    }

//    // 循环分为3层
//    // i表示有5个class
//    // j表示每个class所抽取的样本数量
//    // k表示每个样本的属性数量
//    for (int i = 0; i < 5; i++)
//    {
//        iQuery.bindValue(":id", gnClass[i]);
//        if (false == iQuery.exec())
//        {
//            qDebug() << "Exec error";
//            qDebug() << iQuery.lastError().text();
//            return false;
//        }
//        for (int j = 0; j < NUMBER_OF_TRAINING_SAMPLE_PER_CLASS; j++)
//        {
//            iQuery.next();
//            for (int k = 0; k < ATTRIBUTE_PRE_SAMPLE; k++)
//            {
//                // 读取数据，因为第一列为dataid所以value(j+1)
//                // 加上NUMBER_OF_TRAINING_SAMPLE_PER_CLASS因为每个class取样本的数量
//                iData.at<float>(j+NUMBER_OF_TRAINING_SAMPLE_PER_CLASS*i, k) = static_cast<float>(iQuery.value(k+1).toFloat());
//            }
//            iLabel.at<int>(j+NUMBER_OF_TRAINING_SAMPLE_PER_CLASS*i, 0) = static_cast<int>(iQuery.value(ATTRIBUTE_PRE_SAMPLE+1).toInt());
//        }
//    }
//    qDebug() << "Prepare data finished";
//    return true;
//}


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
        throw "Train failed";
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
void CPredictor::CollectData(eSitType type)
{

    CSerialReader* pReader = CSerialReader::getReader();
    pReader->OpenSerial();
    pReader->ConnectDevice();

    QList< QList<int> > iDataListList;

    // 每个类型的记录条数，用来判断是否达标
    int nTotalNumber = 0;

    qDebug() << "Now collect" <<" SitType ["  <<SitLogic::fetchJudgedMessage(type)<<"] "<< "data, please keep";
    while (nTotalNumber < 1000)
    {
        iDataListList = pReader->ReadSerial();


        for (int j = 0; j < iDataListList.size(); j++)
        {
            for (int k = 0; k < ATTRIBUTE_PRE_SAMPLE; k++)
            {

                // 加上NUMBER_OF_TRAINING_SAMPLE_PER_CLASS因为每个class取样本的数量
                iData.at<float>(j+nTotalNumber, k) = static_cast<float>(iDataListList[j][k]);
            }
            iLabel.at<int>(j+nTotalNumber, 0) = static_cast<int>(type);
        }

        nTotalNumber += iDataListList.size();

    }
    qDebug() << "finished collect" <<" SitType ["  <<SitLogic::fetchJudgedMessage(type)<<"] "<< "data, congratulation";

    pReader->CloseSerial();

}

void CPredictor::loadFromDB()
{

     Predictor p =DAO::query(Session::user);
     pTrees->loadFromString<RTrees>((String)p.xml.toStdString());
}


#include <QFile>
void CPredictor::save2DB()
{
     pTrees->save("temp.xml");

     //read file to database,and delete the temp file.

     Predictor p;
     p.user =Session::user;
     QFile f("temp.xml");
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

void CPredictor::beginReceiveData()
{

}
