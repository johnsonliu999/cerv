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

///
/// \brief The DAO class provide unified interface to operate database.
///
class DAO
{
public:
    static void insert(const Log& log);
    static  QList<Log> query(const QDate& date,const User& user);
    static bool insert(const User& user);
	
    static bool query(const QString& username,int& id);

    static void insert(const Predictor& predictor);
    static Predictor query(const User& user);
};

#endif // DAO_H
