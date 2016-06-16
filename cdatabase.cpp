#include "cdatabase.h"

CDatabase::CDatabase()
{

}

CDatabase::CDatabase(const QString connName, const DBParams &params)
{
    db = QSqlDatabase::addDatabase("QMYSQL", connName);
    db.setDatabaseName(params.database);
    db.setHostName(params.host);
    db.setUserName(params.user);
    db.setPassword(params.password);
    db.setPort(params.port);
}

CDatabase::~CDatabase()
{

}

///
/// \brief CDatabase::getDB return database object.
/// \return Database object.
///
const QSqlDatabase CDatabase::getDB()
{
    if (db.isOpen())
    {
        qDebug() << "Database has already been opened.";
        return db;
    }
    else if (db.open())
    {
        qDebug() << "Opening Database succeed.";
        return db;
    }
    else
        throw QString("Opening database failed, DB cannot use\n" + db.lastError().text());
}
