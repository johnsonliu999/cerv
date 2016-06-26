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
    SitLogic(const User &user);

    static QString fetchJudgedMessage(CPredictor::eSitType sitType);

    CPredictor::eSitType getRecentRes();

private:
    void readOnce(const QString &portName);

public slots:
    void updateSitData(const QString portName);
    void updateModel();

signals:
    void updatePlainText(const QString text);
    void info(const QString title, const QString text);

private:
    QList<CPredictor::eSitType>* mp_resList;
    QList<int>* mp_statList;
    CPredictor* mp_predictor;
    const User m_user;
};


#endif // SITLOGIC_H
