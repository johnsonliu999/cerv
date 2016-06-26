#ifndef CDATABASE_H
#define CDATABASE_H

#include <QtSql>


struct DBParams
{
    QString type;
    QString host;
    QString database;
    QString user;
    QString password;
    int port;

    DBParams(){}

    DBParams(const QString& type, const QString& host, const QString& database,
             const QString& user, const QString& password, const int port)
    {
        this->type = type;
        this->host = host;
        this->database = database;
        this->password = password;
        this->user = user;
        this->port = port;
    }
};

///
/// \brief The CDatabase class provide access to Mysql database.
///
class CDatabase
{
public:
    const QSqlDatabase getDB();
    CDatabase();
    CDatabase(const QString connName, const DBParams& params);
    ~CDatabase();

private:
    QSqlDatabase m_db; ///< Database object. Different thread need different instance.
};

#endif // CDATABASE_H
