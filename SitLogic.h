#ifndef SITLOGIC_H
#define SITLOGIC_H

#include "cpredictor.h"
#include "cserialreader.h"
#include <QString>
#include <QList>
#include <QDebug>

///
/// \brief The SitLogic class
/// process seat related logic.
///
class SitLogic
{
public:
    SitLogic();

    static  QList<CPredictor::eSitType> getSitType();
    static void readOnce();
    static CPredictor::eSitType getAverageType();
    static  QString fetchJudgedMessage(CPredictor::eSitType sitType);

private:
    static    CPredictor* p_predictor;
    static    CSerialReader* p_reader;
    static    QList<CPredictor::eSitType> stType;
};


#endif // SITLOGIC_H
