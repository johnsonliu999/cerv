#if NO
//dao
#include "DAO.h"
#include "cdatabase.h"

///
/// \brief DAO::insert insert log.
/// \param log Log struct.
///
void DAO::insert(QSqlDatabase& db, const Log &log)
{
     db.transaction();
     QSqlQuery query(db);

     query.prepare("INSERT INTO log (date, start_t, end_t,face_type,sit_type,userid) VALUES (:date, :start_t, :end_t,:face_type,:sit_type,:userid)");

     query.bindValue(":date", log.date);
     query.bindValue(":start_t", log.start_t);
     query.bindValue(":end_t", log.end_t);
     query.bindValue(":face_type",log.face_type);
     query.bindValue(":sit_type",log.sit_type);
     query.bindValue(":userid",log.user.userid);

     query.exec();

     db.commit();
}

///
/// \brief DAO::query query log according to data and user.
/// \param[in] date
/// \param[in] user
/// \return Related log.
///
QList<Log> DAO::selectCoordinate(QSqlDatabase& db, const QDate &date,const User &user)
{
    QSqlQuery query(db);
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

    return logs;
}

///
/// \brief DAO::insert insert user.
/// \param user
/// \return success or not.
///
bool DAO::insert(QSqlDatabase& db, const User &user)
{
    int id;

    if(selectCoordinate(db, user.username,id))
        throw QString("user already exist");

    db.transaction();
    QSqlQuery q(db);
    q.prepare("insert into user(username) values(:username)");
    q.bindValue(":username",user.username);
    q.exec();
    db.commit();

    return true;
}

///
/// \brief DAO::query query id according to user name.
/// \param[in] username
/// \param[out] id
/// \return Id of user.
///
bool DAO::selectCoordinate(QSqlDatabase& db, const QString& username, int &id)
{
    QSqlQuery query(db);
    query.prepare("select * from user where username=?");
    query.addBindValue(username);
    query.exec();

    if(query.next())
    {
        id =query.value("id").toInt();
        return true;
    }
    else return false;
}

///
/// \brief DAO::insert insert predictor model.
/// \param[in] predictor predictor object.
///
void DAO::insert(QSqlDatabase &db, const Predictor &predictor)
{
    db.transaction();
    QSqlQuery query(db);

    query.prepare("insert into predictor(xml,userid) values(?,?)");
    QVariant va(predictor.xml);
    query.addBindValue(va);
    query.addBindValue(predictor.user.userid);
    if (!query.exec())
    {
        db.rollback();
        throw QString("Insert predictor error.");
    }

    db.commit();

    qDebug() << "Finished insert predictor model.";
}

///
/// \brief DAO::query query predictor object accroding to user.
/// \param[in] user
/// \return Predictor model.
///
Predictor DAO::selectCoordinate(QSqlDatabase& db, const User &user)
{
    QSqlQuery query(db);
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
        throw QString("No predictor model exist.");

    return p;
}

///
/// \brief DAO::update
/// \param predictor
///
void DAO::update(QSqlDatabase& db, const Predictor& predictor)
{
    QSqlQuery query(db);

    query.prepare("update predictor set xml = ? where userid = ?");
    QVariant va(predictor.xml);
    query.addBindValue(va);
    query.addBindValue(predictor.user.userid);
    if (!query.exec())
    {
        db.close();
        throw QString(query.lastError().text());
    }

    if (query.isActive())
    {
        qDebug() << "" + query.lastQuery();
        qDebug() << query.numRowsAffected() << "rows affected.";
        qDebug() << "update predictor succeed.";
    }
    else
        qDebug() << "query is not active";
}

#include "cfaceclassfier.h"
#include "cv.h"
#include "MySession.h"
void DAO::insert(QSqlDatabase &db, const OrgansCoordinate &coordinate)
{
    QSqlQuery query(db);

    query.prepare("insert into organ_coordinate"
                  "(mouth_x, mouth_y, "
                  "left_eye_x, left_eye_y,"
                  "right_eye_x, right_eye_y,"
                  "user_id) values(?,?,?,?,?,?,?)");
    query.addBindValue(coordinate.mouth.x());
    query.addBindValue(coordinate.mouth.y());
    query.addBindValue(coordinate.leftEye.x());
    query.addBindValue(coordinate.leftEye.y());
    query.addBindValue(coordinate.rightEye.x());
    query.addBindValue(coordinate.rightEye.y());
    query.addBindValue(Session::user.userid);

    if (!query.exec()) throw QString("Insert coordinate failed : "+query.lastError().text());

    qDebug() << "Insert coordinate succeed.";
    qDebug() << query.numRowsAffected() << "rows affected.";
}

OrgansCoordinate DAO::selectCoordinate(QSqlDatabase &db, const User &user)
{
    QSqlQuery query(db);
    query.prepare("select * from predictor where userid = ?");
    query.addBindValue(user.userid);
    if (!query.exec()) throw QString("Query coordinate failed : ")+query.lastError().text();
    if (!query.size()) throw QString("No record.");

    OrgansCoordinate res;
    query.next();
    res.mouth.setX(query.value("mouth_x").toInt());
    res.mouth.setY(query.value("mouth_y").toInt());
    res.mouth.setX(query.value("left_eye_x").toInt());
    res.mouth.setY(query.value("left_eye_y").toInt());
    res.mouth.setX(query.value("right_eye_x").toInt());
    res.mouth.setY(query.value("right_eye_y").toInt());

    return res;
}

void DAO::update(QSqlDatabase &db, const OrgansCoordinate &coordinate)
{
    QSqlQuery query(db);

    query.prepare("update organs_coordinate set mouth_x=?,mouth_y=?,"
                  "left_eye_x=? ,left_eye_y=?,"
                  "right_eye_x=?, right_eye_y=? where user_id=?");
    query.addBindValue(coordinate.mouth.x());
    query.addBindValue(coordinate.mouth.y());
    query.addBindValue(coordinate.leftEye.x());
    query.addBindValue(coordinate.leftEye.y());
    query.addBindValue(coordinate.rightEye.x());
    query.addBindValue(coordinate.rightEye.y());
    query.addBindValue(Session::user.userid);

    if (!query.exec()) throw QString("Update coordinate failed : ")+query.lastError().text();

    qDebug() << "update coordinate succeed.";
    qDebug() << query.numRowsAffected() << "rows affected.";
}

#endif
