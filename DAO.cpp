//dao
#include "DAO.h"
#include "cdatabase.h"






void DAO::insert(const Log &log)
{
     QSqlDatabase db =CDatabase::getDB();
     //do some sql operation
     db.transaction();
     QSqlQuery query;

     query.prepare("INSERT INTO log (date, start_t, end_t,face_type,sit_type,userid) VALUES (:date, :start_t, :end_t,:face_type,:sit_type,:userid)");

     query.bindValue(":date", log.date);
     query.bindValue(":start_t", log.start_t);
     query.bindValue(":end_t", log.end_t);
     query.bindValue(":face_type",log.face_type);
     query.bindValue(":sit_type",log.sit_type);
     query.bindValue(":userid",log.user.userid);

     query.exec();




     db.commit();
     db.close();

}

QList<Log> DAO::query(const QDate &date,const User &user)
{
    QSqlDatabase db =CDatabase::getDB();
    //do some sql operation

    QSqlQuery query;
    query.prepare("select * from log where date=? and userid=?");
    query.addBindValue(date);
    query.addBindValue(user.userid);
    query.exec();

    QList<Log>  logs;

    while (query.next()) {

        Log log;
        log.id        =query.value("id").toInt();
        //log.date      =query.value("date").toDate();
        log.start_t   =query.value("start_t").toTime();
        log.end_t     =query.value("end_t").toTime();
        log.face_type =query.value("face_type").toString();
        log.sit_type  =query.value("sit_type").toString();
        //log.user      =user;

        logs.push_back(log);
    }


    db.close();

    return logs;


}

void DAO::insert(const User &user)
{
    if(query(user.username))
        throw QString("user already exist");
    QSqlDatabase db =CDatabase::getDB();
    //do some sql operation
    db.transaction();
    QSqlQuery query;
    query.prepare("insert into user(username) values(:username)");
    query.bindValue(":username",user.username);
    query.exec();
    db.commit();
    db.close();
}

bool DAO::query(const QString& username)
{
    QSqlDatabase db =CDatabase::getDB();
    //do some sql operation
    QSqlQuery query;
    query.prepare("select * from user where username=?");
    query.addBindValue(username);
    query.exec();

    if(query.next())
        return true;
    else
        return false;


}

void DAO::insert(const Predictor &predictor)
{
    QSqlDatabase db =CDatabase::getDB();
    //do some sql operation
    db.transaction();
    QSqlQuery query;

    query.prepare("insert into table predictor(xml,userid) values(:xml,:userid)");
    query.bindValue(":xml",predictor.xml);
    query.bindValue(":userid",predictor.user.userid);
    query.exec();

    db.commit();
    db.close();
}

Predictor DAO::query(const User &user)
{
    QSqlDatabase db =CDatabase::getDB();
    //do some sql operation

    QSqlQuery query;
    query.prepare("select * from predictor where userid =:userid");
    query.bindValue(":userid",user.userid);
    query.exec();

    Predictor p;
    if(query.next())
    {

        p.id =query.value("id").toInt();
        p.user =user;
        p.xml =query.value("xml").toByteArray();
    }

     db.close();
     return p;
}

