//dao
#include "DAO.h"
#include "cdatabase.h"

///
/// \brief DAO::insert insert log.
/// \param log Log struct.
///
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

///
/// \brief DAO::query query log according to data and user.
/// \param[in] date
/// \param[in] user
/// \return Related log.
///
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

///
/// \brief DAO::insert insert user.
/// \param user
/// \return success or not.
///
bool DAO::insert(const User &user)
{
    int id;
    if(query(user.username,id))
        throw QString("user already exist");

    QSqlDatabase db =CDatabase::getDB();
    //do some sql operation
    db.transaction();
    QSqlQuery q;
    q.prepare("insert into user(username) values(:username)");
    q.bindValue(":username",user.username);
    q.exec();
    db.commit();
    db.close();

    return true;
}

///
/// \brief DAO::query query id according to user name.
/// \param[in] username
/// \param[out] id
/// \return Id of user.
///
bool DAO::query(const QString& username, int &id)
{
    QSqlDatabase db =CDatabase::getDB();
    //do some sql operation
    QSqlQuery query;
    query.prepare("select * from user where username=?");
    query.addBindValue(username);
    query.exec();

    if(query.next())
    {
        id =query.value("id").toInt();
        db.close();
        return true;
    }
    else
    {
        db.close();
        return false;
    }


}

///
/// \brief DAO::insert insert predictor model.
/// \param[in] predictor predictor object.
///
void DAO::insert(const Predictor &predictor)
{
    QSqlDatabase db =CDatabase::getDB();
    //do some sql operation
    db.transaction();
    QSqlQuery query;

    query.prepare("insert into predictor(xml,userid) values(?,?)");
    QVariant va(predictor.xml);
    query.addBindValue(va);
    query.addBindValue(predictor.user.userid);
    query.exec();

    db.commit();
    db.close();

    qDebug() << "Finished insert predictor model.";
}

///
/// \brief DAO::query query predictor object accroding to user.
/// \param[in] user
/// \return Predictor model.
///
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
    else
    {
        db.close();
        throw QString("No predictor model exist.");
    }

    db.close();
    return p;
}

