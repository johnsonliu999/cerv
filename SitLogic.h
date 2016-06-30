#ifndef SITLOGIC_H
#define SITLOGIC_H


#include "cserialreader.h"
#include "cpredictor.h"

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

    static QString Enum2String(Sit::SitType sitType);

    void start();
    void stop();


private:
    void readOnce(const QString &portName);
    Sit::SitType getRecentRes();

public slots:
    void updateSitRes(const QString portName);
    void getRecentType(Sit::SitType &type);

signals:
    void updateDisp(const QString text);
    void info(const QString title, const QString text);

private:
    QList<Sit::SitType>* mp_resList;
    QList<int>* mp_statList;
    CPredictor* mp_predictor;
};


#endif // SITLOGIC_H
