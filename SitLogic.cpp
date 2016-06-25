#include "SitLogic.h"

QList<CPredictor::eSitType> SitLogic::resList = QList<CPredictor::eSitType>();
CPredictor* SitLogic::p_predictor =CPredictor::getPredictor();

const int RES_NUM = 5; ///< the number of results kept by stType
const int TYPE_NUM = 5;

///
/// \brief SitLogic::SitLogic
/// initiate the statistic list
///
SitLogic::SitLogic() :
    p_statList(new QList<int>)
{
    for (int i = 0; i < TYPE_NUM; i++)
        p_statList << 0;
}

///
/// \brief SitLogic::getSitType return list of recent sit type.
/// \return List of recent seat type.
///
CPredictor::eSitType SitLogic::getSitType()
{
    int ind = 0;
    for (int i = 0; i < TYPE_NUM; i++)
    {
        if (p_statList[ind] < p_statList[i])
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
    for(auto data : reader.readSerial())
    {
        if (resList.size() >= RES_NUM)
            resList
        resList.append(p_predictor->Predict(data));
    }
}

///
/// \brief SitLogic::getAverageType combine recent seat type into a single one.
/// \return Fused type.
///
CPredictor::eSitType SitLogic::getAverageType()
{
    if (resList.size() != 0)
        return resList.at(0);
    else
        return CPredictor::eSitType::UNKNOWN;
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
