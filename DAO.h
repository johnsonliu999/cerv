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

    static void insert(Log log,User user);
    static  QList<Log> query(QDate date,User user);

    static void insert(User user);
    static User query(QString username);



    static void insert(Predictor predictor,User user);
    static Predictor query(User user);

private:
    DAO();
};

#endif // DAO_H
