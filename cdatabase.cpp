#include "cdatabase.h"

CDatabase::CDatabase()
{

}

CDatabase::CDatabase(const QString connName, const DBParams &params)
{
    m_db = QSqlDatabase::addDatabase("QMYSQL", connName);
    m_db.setDatabaseName(params.database);
    m_db.setHostName(params.host);
    m_db.setUserName(params.user);
    m_db.setPassword(params.password);
    m_db.setPort(params.port);
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
    if (m_db.isOpen())
    {
        qDebug() << "Database has already been opened.";
        return m_db;
    }
    else if (m_db.open())   return m_db;
    else    throw QString("Opening database failed, DB cannot use\n" + m_db.lastError().text());
}
