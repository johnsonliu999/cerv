#ifndef DAO_H
#define DAO_H



#include "Log.h"
#include "Predictor.h"
#include "User.h"
#include <QList>
#include <QDate>
#include <QString>

class QSqlDatabase;

/**
   all data access interface defined here.
*/

///
/// \brief The DAO class provide unified interface to operate database.
///
class DAO
{
public:
    static void insert(QSqlDatabase &db, const Log& log);
    static  QList<Log> query(QSqlDatabase& db, const QDate& date, const User& user);
    static bool insert(QSqlDatabase &db, const User& user);
	
    static bool query(QSqlDatabase& db, const QString& username,int& id);

    static void insert(QSqlDatabase& db, const Predictor& predictor);
    static Predictor query(QSqlDatabase& db, const User& user);
    static void update(QSqlDatabase& db, const Predictor &predictor);
};

#endif // DAO_H
