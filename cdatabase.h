#ifndef CDATABASE_H
#define CDATABASE_H

#include <QtSql>

struct Log;
struct OrgansCoordinate;

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
    CDatabase(const QString connName, const DBParams& params = DBParams("QMYSQL", "localhost", "neck", "root", "qq452977491", 3306));
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

    QList<Log> selectLog(const int limit = 10);
    QList<Log> selectLog(const QDate &date, const int limit = 10);
    void insertLog(const Log &log);

private:
    QSqlDatabase m_db; ///< Database object. Different thread need different instance.
};

#endif // CDATABASE_H
