#include "logger.h"

Logger::Logger(const FaceLogic *p_faceLogic,
                     const SitLogic *p_sitLogic, QObject *parent) :
    QObject(parent),
    mp_faceLogic(p_faceLogic),
    mp_sitLogic(p_sitLogic),
    mp_db(new CDatabase("logger", DBParams("MYSQL", "localhost", "neck", "root", "qq452977491", 3306)))
{
    connect(this, &Logger::getRecentFaceType, mp_faceLogic, &FaceLogic::getRecentType, Qt::BlockingQueuedConnection);
    connect(this, &Logger::getRecentSitType, mp_sitLogic, &SitLogic::getRecentType, Qt::BlockingQueuedConnection);
}

Logger::~Logger()
{

}

void Logger::log()
{
    qDebug() << "logging";
    Face::FaceType faceType;
    Sit::SitType sitType;
    emit getRecentFaceType(faceType);
    emit getRecentSitType(sitType);
    Log log;
    log.faceType = faceType;
    log.sitType = sitType;
    mp_db->insertLog(log);
}

void Logger::start()
{
    qDebug() << "Start Logger";
    try {
        mp_db->openDB();
    } catch(const QString &e)
    {
        qDebug() << "Logger::start : can not open DB";
        throw e;
    }

    mp_thread = new QThread;
    this->moveToThread(mp_thread);
    mp_thread->start();

    mp_timer = new QTimer();
    connect(mp_timer, &QTimer::timeout, this, &Logger::log);
    mp_timer->start(20*1000);
}

void Logger::stop()
{
    qDebug() << "Stop logger";
    mp_db->closeDB();
    if (mp_thread)
    {
        mp_timer->stop();
        disconnect(mp_timer, &QTimer::timeout, this, &Logger::log);
        mp_timer->~QTimer();

        mp_thread->quit();
        mp_thread->wait();
        mp_thread->~QThread();
    }
}
