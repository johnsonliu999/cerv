#include "SitLogic.h"
#include "MySession.h"

const int RES_NUM = 5; ///< the number of results kept by stType
///
/// \brief SitLogic::SitLogic
/// initialize the statistic list
///
SitLogic::SitLogic() :
    mp_statList(new QList<int>),
    mp_resList(new QList<Sit::SitType>),
    mp_predictor(new CPredictor)
{
    for (int i = 0; i < CPredictor::TYPE_NUM; i++)
        *mp_statList << 0;
}

///
/// \brief SitLogic::getSit::Sit::SitType return list of recent sit type.
/// \return List of recent seat type.
///
Sit::SitType SitLogic::getRecentRes()
{
    int ind = 0;
    for (int i = 0; i < CPredictor::TYPE_NUM; i++)
    {
        if ((*mp_statList)[ind] < (*mp_statList)[i])
            ind = i;
    }

    return Sit::SitType(ind);
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
        Sit::SitType res = mp_predictor->classify(records[i]);
        mp_resList->append(res);
        (*mp_statList)[(int)res]++;
    }
}

///
/// \brief SitLogic::updateSitRes
/// update recent result of classification.
/// Wait for [updatePlainText] to use.
///
/// Time cost : 2445 msecs
///
/// \param portName
///
void SitLogic::updateSitRes(const QString portName)
{
    qDebug() << "updateSitRes called";

    QTime t1 = QTime::currentTime();
    try {
        readOnce(portName);
    } catch (const QString& e)
    {
        emit info("readOnce", e);
        return;
    }
    QTime t2 = QTime::currentTime();
    qDebug() << "Sit time cost :" << t1.msecsTo(t2);

    emit updateDisp(Enum2String(getRecentRes()));
}

void SitLogic::getRecentType(Sit::SitType &type)
{
    type = getRecentRes();
}

///
/// \brief SitLogic::fetchJudgedMessage provide sit type in string format.
/// \param Sit::SitType
/// \return Seat type in string format.
///
QString SitLogic::Enum2String(Sit::SitType sitType)
{
    switch (sitType) {
    case Sit::NORMAL:
        return "Normal";
        break;
    case Sit::LEFTWARD:
        return "Leftward";
        break;
    case Sit::RIGHTWARD:
        return "Rightward";
        break;
    case Sit::BACKWARD:
        return "Backward";
        break;
    case Sit::FORWARD:
        return "Forward";
        break;
    case Sit::UNKNOWN:
        return "Unknown";
        break;
    }
}

void SitLogic::start()
{
    for (int i = 0; i < RES_NUM; i++)
        (*mp_statList)[i] = 0;

    mp_resList->clear();

    try{
        mp_predictor->loadFromDB();
    } catch(const QString &e)
    {
        qDebug() << "start:failed to load predictor.";
        throw e;
    }
}

void SitLogic::stop()
{

}
