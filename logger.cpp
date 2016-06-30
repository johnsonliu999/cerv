#include "logger.h"

Logger::Logger(const FaceLogic *p_faceLogic,
                     const SitLogic *p_sitLogic, QObject *parent) :
    QObject(parent),
    mp_faceLogic(p_faceLogic),
    mp_sitLogic(p_sitLogic),
    mp_db(new CDatabase("logger", DBParams("MYSQL", "localhost", "neck", "root", "qq452977491", 3306)))
{
    connect(this, &Logger::getRecentFace::FaceType, mp_faceLogic, &FaceLogic::getRecentType, Qt::BlockingQueuedConnection);
    connect(this, &Logger::getRecentSit::Sit::SitType, mp_sitLogic, &SitLogic::getRecentType, Qt::BlockingQueuedConnection);
}

void Logger::log()
{
    Face::FaceType Face::FaceType;
    Sit::SitType Sit::SitType;
    emit getRecentFace::FaceType(Face::FaceType);
    emit getRecentSit::Sit::SitType(Sit::SitType);
    Log log;
    log.Face::FaceType = Face::FaceType;
    log.Sit::SitType = Sit::SitType;
    mp_db->insertLog(log);
}

void Logger::start()
{
    try {
        mp_db->openDB();
    } catch(const QString &e)
    {
        qDebug() << "Logger::start : can not open DB";
        throw e;
    }

    mp_timer = new QTimer();
    connect(mp_timer, &QTimer::timeout, this, &Logger::log);
    mp_timer->start(20*1000);
}

void Logger::stop()
{
    mp_db->close();
    mp_timer->stop();
    disconnect(mp_timer, &QTimer::timeout, this, &Logger::log);
    mp_timer->~QTimer();
}
