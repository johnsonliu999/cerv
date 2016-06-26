#include "SitLogic.h"

const int RES_NUM = 5; ///< the number of results kept by stType
const int TYPE_NUM = 5;

/// SitLogic is mainly used for processing 'sit' related operation.
/// * SitLogic object is bound with a user.



///
/// \brief SitLogic::SitLogic
/// initialize the statistic list
///
SitLogic::SitLogic(const User& user) :
    mp_statList(new QList<int>),
    mp_resList(new QList<CPredictor::eSitType>),
    mp_predictor(new CPredictor),
    m_user(user)
{
    for (int i = 0; i < TYPE_NUM; i++)
        *mp_statList << 0;
}

///
/// \brief SitLogic::getSitType return list of recent sit type.
/// \return List of recent seat type.
///
CPredictor::eSitType SitLogic::getRecentRes()
{
    int ind = 0;
    for (int i = 0; i < TYPE_NUM; i++)
    {
        if ((*mp_statList)[ind] < (*mp_statList)[i])
            ind = i;
    }

    return CPredictor::eSitType(ind);
}

///
/// \brief SitLogic::readOnce
/// read and store recent predicted type into stType.
///
void SitLogic::readOnce(const QString& portName)
{
    CSerialReader reader(portName);
    QSerialPort* p_port;

    try{
        p_port = reader.openPort();
    } catch(const QString& e)
    {
        emit info("readOnce", e);
        return ;
    }

    QList<QList<int> > records;
    try{
        records = reader.getTrainData();
    } catch(const QString& e)
    {
        p_port->close();
        emit info("readOnce", e);
        return ;
    }

    p_port->close();

    // update statistics
    for(int i = 0; i < records.length(); i++)
    {
        if (mp_resList->size() >= RES_NUM)
        {
            (*mp_statList)[(int)(*mp_resList)[0]]--;
            mp_resList->removeFirst();
        }
        CPredictor::eSitType res = mp_predictor->predict(records[i]);
        mp_resList->append(res);
        (*mp_statList)[(int)res]++;
    }
}

///
/// \brief SitLogic::updateSitData
/// Time cost : 2445 msecs
///
/// \param portName
///
void SitLogic::updateSitData(const QString portName)
{
    qDebug() << "updateSitData called";

    QTime t1 = QTime::currentTime();
    try {
        readOnce(portName);
    } catch (const QString& e)
    {
        emit info("readOnce", e);
        return;
    }
    QTime t2 = QTime::currentTime();
    qDebug() << "time cost :" << t1.msecsTo(t2);

    emit updatePlainText(fetchJudgedMessage(getRecentRes()));
}

void SitLogic::updateModel()
{
    try {
        mp_predictor->loadFromDB(m_user);
    } catch(const QString& e)
    {
        emit info("updateModel", e);
    }
}

///
/// \brief SitLogic::fetchJudgedMessage provide sit type in string format.
/// \param sitType
/// \return Seat type in string format.
///
QString SitLogic::fetchJudgedMessage(CPredictor::eSitType sitType)
{
    switch (sitType) {
    case CPredictor::NORMAL:
        return "Normal";
        break;
    case CPredictor::LEFTWARD:
        return "Leftward";
        break;
    case CPredictor::RIGHTWARD:
        return "Rightward";
        break;
    case CPredictor::BACKWARD:
        return "Backward";
        break;
    case CPredictor::FORWARD:
        return "Forward";
        break;
    case CPredictor::UNKNOWN:
        return "Unknown";
        break;
    }
}
