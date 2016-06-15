#include "cdatabase.h"


QSqlDatabase CDatabase::db =QSqlDatabase();

CDatabase::CDatabase()
{

}

///
/// \brief CDatabase::__ConnectDatabase
/// connect database.
///
/// \param cDatabaseType Type of Database.
/// \param cHostName Host name.
/// \param cDatabaseName
/// \param cUserName
/// \param cPassword
///
void CDatabase::__ConnectDatabase(const QString &cDatabaseType  = "QMYSQL", const QString &cHostName = "localhost",
                                  const QString &cDatabaseName = "neck", const QString &cUserName = "root",
                                  const QString &cPassword = "qq452977491")
{
    db = QSqlDatabase::addDatabase(cDatabaseType);
    db.setHostName(cHostName);
    db.setDatabaseName(cDatabaseName);
    db.setUserName(cUserName);
    db.setPassword(cPassword);
    if (db.open())
        qDebug() << "Connect database succeed";
    else
        throw QString("Connect database failed, DB cannot use\n" + db.lastError().text());
}

///
/// \brief CDatabase::getDB return database object.
/// \return Database object.
///
const QSqlDatabase CDatabase::getDB()
{
    if (db.isValid())
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
    else
    {
       __ConnectDatabase();

       // after connect, should use the new create connection
       db = QSqlDatabase::database();
       return db;
    }
}

void CDatabase::CloseDB()
{
    db.close();
}
