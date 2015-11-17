#ifndef DAO_H
#define DAO_H



#include "Log.h"
#include "Predictor.h"
#include "User.h"
#include <QList>
#include <QDate>
#include <QString>

/**
   all data access interface defined here.
*/


class DAO
{
public:

    static void insert(Log& log);
    static  QList<Log> query(QDate& date,User& user);

    static void insert(User& user);
    static bool query(QString& username);
	
    static void insert(Predictor& predictor);
    static Predictor query(User& user);

};

#endif // DAO_H
