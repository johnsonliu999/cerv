#include "SitLogic.h"

CPredictor* SitLogic::p_predictor =CPredictor::getPredictor();

const int RES_NUM = 5; ///< the number of results kept by stType
const int TYPE_NUM = 5;

///
/// \brief SitLogic::SitLogic
/// initiate the statistic list
///
SitLogic::SitLogic() :
    p_statList(new QList<int>),
    p_resList(new QList<CPredictor::eSitType>)
{
    for (int i = 0; i < TYPE_NUM; i++)
        *p_statList << 0;
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
        if ((*p_statList)[ind] < (*p_statList)[i])
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
    for(auto data : reader.readSerial())
    {
        if (p_resList->size() >= RES_NUM)
        {
            (*p_statList)[(int)(*p_resList)[0]]--;
            p_resList->removeFirst();
        }
        CPredictor::eSitType res = p_predictor->Predict(data);
        p_resList->append(res);
        (*p_statList)[(int)res]++;
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
