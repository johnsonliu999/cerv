#include "cdatabase.h"


QSqlDatabase CDatabase::db =QSqlDatabase();
CDatabase::CDatabase()
{

}

void CDatabase::__ConnectDatabase(const QString &cDatabaseType, const QString &cHostName, const QString &cDatabaseName, const QString &cUserName, const QString &cPassword)
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

const QSqlDatabase CDatabase::getDB()
{
    if (db.isValid())
    {
        if (db.isOpen() || db.open())
        {
            qDebug() << "Connect database succeed";
            return db;
        }
        throw QString("Connect database failed, DB cannot use\n" + db.lastError().text());
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


