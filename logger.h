#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>

#include "FaceLogic.h"
#include "SitLogic.h"
#include "cdatabase.h"

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
    FaceLogic *mp_faceLogic;
    SitLogic *mp_sitLogic;
    CDatabase *mp_db;
    QTimer *mp_timer;

signals:
    void getRecentFaceType(Face::FaceType &faceType);
    void getRecentSitType(Sit::SitType &sitType);

public slots:
    void start();
    void stop();

private slots:
    void log();
};

#endif // LOGGER_H
