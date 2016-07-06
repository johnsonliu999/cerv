#include "loadder.h"
#include "cdatabase.h"
#include "logger.h"

#include <QThread>


Loadder::Loadder(QObject *parent) : QObject(parent),
    mp_thread(new QThread(this))
{
    qDebug() << "Loadder::Loadder";
    moveToThread(mp_thread);
    mp_thread->start();
}

Loadder::~Loadder()
{

}

void Loadder::loadLog(const QDate &date)
{
    qDebug() << "called loadLog";
    CDatabase db("loadder");
    try {
        db.openDB();
    } catch (const QString &e)
    {
        qDebug() << "Loadder::loadLog():cannot open DB.";
        qDebug() << e;
    }
    QList<Log> logList;
    try{
        logList = db.selectLog(date);
    } catch (const QString &e)
    {
        qDebug() << "Loadder::loadLog():cannot select log";
        db.closeDB();
        mp_thread->quit();
        mp_thread->wait();
        emit info("Loadder::loadLog", e);
        emit finishLoad();
        return ;
    }

    db.closeDB();
    mp_thread->quit();
    mp_thread->wait();
    emit updateLogTable(logList);
    emit finishLoad();
}

void Loadder::loadLog(const QDate &from, const QDate &to)
{
    qDebug() << "called loadLog form to";
    CDatabase db("loadder");
    try {
        db.openDB();
    } catch (const QString &e)
    {
        qDebug() << "Loadder::loadLog():cannot open DB.";
        qDebug() << e;
    }
    QList<Log> logList;
    try{
        logList = db.selectLog(from, to);
    } catch (const QString &e)
    {
        qDebug() << "Loadder::loadLog():cannot select log";
        db.closeDB();
        mp_thread->quit();
        mp_thread->wait();
        emit infoBarChart("Loadder::loadLog", e);
        return ;
    }

    db.closeDB();

    qDebug() << "Size:" << logList.size();

    QVector<double> freqs(6);
    for (int i = 0; i < logList.size(); i++)
    {
        Log log = logList[i];
        freqs[(int)log.faceType]++;
        freqs[(int)log.sitType]++;
    }
    qDebug() << "Loadder:" << freqs;
    emit updateBarChart(freqs);

    int maxIndex = 0;
    for (int i = 0; i < 6; i++)
    {
        if (freqs[maxIndex] < freqs[i])
            maxIndex = i;
    }

    QString showString = "You tend to be " + CFaceClassfier::Enum2String((Face::FaceType)maxIndex);

    emit updateShow(showString);
    mp_thread->quit();
    mp_thread->wait();

}
