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
    static void __ConnectDatabase(const QString & cDatabaseType, const QString & cHostName,
                                  const QString & cDatabaseName, const QString & cUserName,
                                  const QString & cPassword);
};

#endif // CDATABASE_H
