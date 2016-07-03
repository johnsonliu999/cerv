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
        emit finishedLoad();
        return ;
    }

    db.closeDB();
    mp_thread->quit();
    mp_thread->wait();
    emit updateLogTable(logList);
    emit finishedLoad();
}
