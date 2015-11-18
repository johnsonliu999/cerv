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
    static bool __ConnectDatabase(const QString & cDatabaseType = "QMYSQL", const QString & cHostName = "localhost", const QString & cDatabaseName = "neck", const QString & cUserName = "root", const QString & cPassword = "19940116");
};

#endif // CDATABASE_H
