#ifndef CDATABASE_H
#define CDATABASE_H

#include <QtSql>
#include "cfaceclassfier.h"


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

struct User;

///
/// \brief The CDatabase class provide access to Mysql database.
///
class CDatabase
{
public:
    CDatabase();
    CDatabase(const QString connName, const DBParams& params);
    ~CDatabase();

    void openDB();
    void closeDB();

    int selectUserId(const QString &username);
    void insertUserName(const QString &username);

    QByteArray selectXml();
    void insertXml(const QByteArray& xml);
    void updateXml(const QByteArray& xml);

    OrgansCoordinate selectCoordinate();
    void insertCoordinate(const OrgansCoordinate &coordinate);
    void updateCoordinate(const OrgansCoordinate &coordinate);






private:
    QSqlDatabase m_db; ///< Database object. Different thread need different instance.
};

#endif // CDATABASE_H
