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

    CPredictor::eSitType getRecentRes();
    void readOnce(const QString &portName);
    static QString fetchJudgedMessage(CPredictor::eSitType sitType);

private:
    static    CPredictor* p_predictor;
    QList<CPredictor::eSitType>* p_resList;
    QList<int>* p_statList;
};


#endif // SITLOGIC_H
