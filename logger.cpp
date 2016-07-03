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
    try{
        mp_db->insertLog(log);
    } catch (const QString &e)
    {
        qDebug() << "Logger::log : Insert log failed.";
        qDebug() << "Logger::log :" << e;
    }
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

    mp_thread = new QThread(this);
    mp_thread->start();
    mp_timer = new QTimer(this);
    mp_timer->setInterval(20*1000);

    moveToThread(mp_thread);
    connect(mp_timer, &QTimer::timeout, this, &Logger::log);
    connect(this, SIGNAL(startTimer()), mp_timer, SLOT(start()));
    emit startTimer();
}

void Logger::stop()
{
    qDebug() << "Stop logger";
    mp_db->closeDB();
    if (mp_thread)
    {
        mp_timer->stop();
        disconnect(mp_timer, &QTimer::timeout, this, &Logger::log);

        mp_thread->quit();
        mp_thread->wait();
    }
    emit stopped();
}
