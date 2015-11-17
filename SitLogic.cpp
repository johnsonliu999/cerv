#include "SitLogic.h"




QList<CPredictor::eSitType> SitLogic::stType = QList<CPredictor::eSitType>();
CPredictor* SitLogic::predictor =CPredictor::getPredictor();
CSerialReader* SitLogic::reader =CSerialReader::getReader();




void SitLogic::init()
{

    if(!reader->OpenSerial())
    {
        qDebug() << "Cannot open serial port";

    }
    qDebug() << "Open serial port succeed";

    if(!reader->ConnectDevice())
    {
        qDebug() << "Cannot connect device";

    }
    qDebug() << "Connect device succeed";



}

void SitLogic::readOnce()
{
    for(auto data : reader->ReadSerial())
         stType.append(predictor->Predict(data));
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
