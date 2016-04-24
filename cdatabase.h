#ifndef CDATABASE_H
#define CDATABASE_H

#include <QtSql>

///
/// \brief The CDatabase class provide access to Mysql database.
///
class CDatabase
{
public:
    static const QSqlDatabase getDB();
    static void CloseDB();
private:
    CDatabase();
    static QSqlDatabase db; ///< Database object.
    static void __ConnectDatabase(const QString & cDatabaseType = "QMYSQL", const QString & cHostName = "localhost",
                                  const QString & cDatabaseName = "neck", const QString & cUserName = "root",
                                  const QString & cPassword = "qq452977491");
};

#endif // CDATABASE_H
