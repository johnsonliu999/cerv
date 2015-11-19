#include "SitLogic.h"

QList<CPredictor::eSitType> SitLogic::stType = QList<CPredictor::eSitType>();
CPredictor* SitLogic::predictor =CPredictor::getPredictor();
CSerialReader* SitLogic::reader =CSerialReader::getReader();

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

void SitLogic::readOnce()
{
    for(auto data : reader->ReadSerial())
        stType.append(predictor->Predict(data));
}

CPredictor::eSitType SitLogic::getAverageType()
{
     return stType.at(0);
}

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
