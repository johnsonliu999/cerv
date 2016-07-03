#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>

#include "FaceLogic.h"
#include "SitLogic.h"

class CDatabase;
class QTread;

struct Log
{
    QTime time;
    QDate date;
    Face::FaceType faceType;
    Sit::SitType sitType;
    int type;
};

class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(const FaceLogic *p_faceLogic,
                    const SitLogic *p_sitLogic,
                    QObject *parent = 0);
    ~Logger();



private:
    const FaceLogic *mp_faceLogic;
    const SitLogic *mp_sitLogic;
    CDatabase *mp_db;
    QTimer *mp_timer;
    QThread *mp_thread;

signals:
    void getRecentFaceType(Face::FaceType &faceType);
    void getRecentSitType(Sit::SitType &sitType);
    void startTimer();
    void stopped();

public slots:
    void start();
    void stop();

private slots:
    void log();
};

#endif // LOGGER_H
