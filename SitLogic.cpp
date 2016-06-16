#include "SitLogic.h"

QList<CPredictor::eSitType> SitLogic::stType = QList<CPredictor::eSitType>();
CPredictor* SitLogic::p_predictor =CPredictor::getPredictor();

///
/// \brief SitLogic::getSitType return list of recent sit type.
/// \return List of recent seat type.
///
QList<CPredictor::eSitType> SitLogic::getSitType()
{
     return stType;
}

///
/// \brief SitLogic::readOnce
/// read and store recent predicted type into stType.
///
void SitLogic::readOnce(const QString& portName)
{
    CSerialReader reader(portName);
    QSerialPort* p_port = reader.getPort();

    for(auto data : reader.readSerial())
        stType.append(p_predictor->Predict(data));

    p_port->close();
}

///
/// \brief SitLogic::getAverageType combine recent seat type into a single one.
/// \return Fused type.
///
CPredictor::eSitType SitLogic::getAverageType()
{
    if (stType.size() != 0)
        return stType.at(0);
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
