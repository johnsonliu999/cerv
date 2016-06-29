#if NO
#ifndef LOG_H
#define LOG_H



#include "User.h"
#include <QDate>
#include <QTime>
#include <QString>





/**
   domain object ,represent a record of database table log.

*/
struct  Log
{

public:
    int id;
    QDate  date;
    QTime  start_t;
    QTime  end_t;

    QString face_type;
    QString sit_type;

    User user;



};



#endif // LOG_H
#endif
