#ifndef CDATABASE_H
#define CDATABASE_H

#include <QtSql>


class CDatabase
{
public:
    static const QSqlDatabase getDB();
    static void CloseDB();
private:
    CDatabase();
    static QSqlDatabase db;
<<<<<<< HEAD
    static void __ConnectDatabase(const QString & cDatabaseType, const QString & cHostName,
                                  const QString & cDatabaseName, const QString & cUserName,
                                  const QString & cPassword);
=======
    static void __ConnectDatabase(const QString & cDatabaseType = "QMYSQL", const QString & cHostName = "localhost", const QString & cDatabaseName = "neck", const QString & cUserName = "root", const QString & cPassword = "qq452977491");
>>>>>>> develop
};

#endif // CDATABASE_H
