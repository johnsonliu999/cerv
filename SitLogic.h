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

    static  CPredictor::eSitType getSitType();
    static void readOnce(const QString &portName);
    static CPredictor::eSitType getAverageType();
    static  QString fetchJudgedMessage(CPredictor::eSitType sitType);

private:
    static    CPredictor* p_predictor;
    static    QList<CPredictor::eSitType> resList;
    QList<int>* p_statList;
};


#endif // SITLOGIC_H
