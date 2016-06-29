#ifndef SITLOGIC_H
#define SITLOGIC_H

#include "cpredictor.h"
#include "cserialreader.h"
#include <QString>
#include <QList>
#include <QDebug>
#include <QObject>

///
/// \brief The SitLogic class
/// process seat related logic.
///
class SitLogic : public QObject
{
    Q_OBJECT
public:
    SitLogic();

    static QString Enum2String(CPredictor::SitType sitType);

    void start();
    void stop();


private:
    void readOnce(const QString &portName);
    CPredictor::SitType getRecentRes();

public slots:
    void updateSitRes(const QString portName);

signals:
    void updateDisp(const QString text);
    void info(const QString title, const QString text);

private:
    QList<CPredictor::SitType>* mp_resList;
    QList<int>* mp_statList;
    CPredictor* mp_predictor;
};


#endif // SITLOGIC_H
