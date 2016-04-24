#include "SitLogic.h"

QList<CPredictor::eSitType> SitLogic::stType = QList<CPredictor::eSitType>();
CPredictor* SitLogic::predictor =CPredictor::getPredictor();
CSerialReader* SitLogic::reader =CSerialReader::getReader();

///
/// \brief SitLogic::getSitType return list of recent sit type.
/// \return List of recent seat type.
///
QList<CPredictor::eSitType> SitLogic::getSitType()
{
     return stType;
}

/**
* @brief SitLogic::init
* open the bluetooth serial port
* and connect the slave bluetooth device
*/
void SitLogic::init()
{
    reader->OpenSerial(BT_ID);
    reader->ConnectDevice(BT_ADDR);
}

///
/// \brief SitLogic::readOnce
/// read and store recent predicted type into stType.
///
void SitLogic::readOnce()
{
    for(auto data : reader->ReadSerial())
        stType.append(predictor->Predict(data));
}

///
/// \brief SitLogic::getAverageType combine recent seat type into a single one.
/// \return Fused type.
///
CPredictor::eSitType SitLogic::getAverageType()
{
     return stType.at(0);
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
    }
}
