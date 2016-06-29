#include "cdatabase.h"
#include "MySession.h"

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

void CDatabase::closeDB()
{
    m_db.close();
}

int CDatabase::selectUserId(const QString &username)
{
    QSqlQuery query(m_db);
    query.prepare("select * from user where username=?");
    query.addBindValue(username);
    if (!query.exec())
        throw QString("[selectUserId]:")+query.lastError().text();

    if (!query.size()) return -1;

    query.next();
    return query.value("id").toInt();
}

void CDatabase::insertUserName(const QString &username)
{
    QSqlQuery query(m_db);
    query.prepare("insert into user(username) values(?)");
    query.addBindValue(username);
    if (!query.exec())
        throw QString("[insertUserName]:")+query.lastError().text();

    qDebug() << "Insert user name succeed";
}

QByteArray CDatabase::selectXml()
{
    QSqlQuery query(m_db);
    query.prepare("select xml from predictor where user_id=?");
    query.addBindValue(Session::UserId);
    if (!query.exec())
        throw QString("selectXml:")+query.lastError().text();
    if (!query.size())
        throw QString("selectXml:No record");

    query.next();
    return query.value("xml").toByteArray();
}

void CDatabase::insertXml(const QByteArray &xml)
{
    QSqlQuery query(m_db);
    query.prepare("insert into predictor(xml, userid) values(?,?)");
    query.addBindValue(xml);
    query.addBindValue(Session::UserId);
    if (!query.exec())
        throw QString("insertXml:")+query.lastError().text();

    qDebug() << "Insert xml succeed.";
    qDebug() << query.numRowsAffected() << "rows affected";
}

void CDatabase::updateXml(const QByteArray &xml)
{
    QSqlQuery query(m_db);
    query.prepare("update predictor set xml=? where userid=?");
    query.addBindValue(xml);
    if (!query.exec())
        throw QString("updateXml:")+query.lastError().text();
    qDebug() << "Update xml succeed.";
    qDebug() << query.numRowsAffected() << "rows affected";
}

OrgansCoordinate CDatabase::selectCoordinate()
{
    QSqlQuery query(m_db);
    query.prepare("select * from organs_coordinate where user_id=?");
    query.addBindValue(Session::UserId);
    if (!query.exec())
        throw QString("selectCoordinate:")+query.lastError().text();
    if (!query.size())
        throw QString("selectCoordinate:No record.");
    OrgansCoordinate res;
    query.next();
    res.leftEye.setX(query.value("left_eye_x").toInt());
    res.leftEye.setY(query.value("left_eye_y").toInt());
    res.rightEye.setX(query.value("right_eye_x").toInt());
    res.rightEye.setY(query.value("right_eye_y").toInt());
    res.mouth.setX(query.value("mouth_x").toInt());
    res.mouth.setY(query.value("mouth_y").toInt());
    return res;
}

void CDatabase::insertCoordinate(const OrgansCoordinate &coordinate)
{
    QSqlQuery query(m_db);
    query.prepare("insert into organs_coordinate"
                  "(mouth_x, mouth_y,"
                  "left_eye_x, left_eye_y,"
                  "right_eye_x, right_eye_y) "
                  "where user_id=?");
    query.addBindValue(coordinate.mouth.x());
    query.addBindValue(coordinate.mouth.y());
    query.addBindValue(coordinate.leftEye.x());
    query.addBindValue(coordinate.leftEye.y());
    query.addBindValue(coordinate.rightEye.x());
    query.addBindValue(coordinate.rightEye.y());
    query.addBindValue(Session::UserId);
    if (!query.exec())
        throw QString("insertCoordinate:")+query.lastError().text();
    qDebug() << "Insert Coordinate succeed.";
    qDebug() << query.numRowsAffected() << "rows affected";
}

void CDatabase::updateCoordinate(const OrgansCoordinate &coordinate)
{
    QSqlQuery query(m_db);
    query.prepare("update organs_coordinate set "
                  "left_eye_x=?, left_eye_y=?,"
                  "right_eye_x=?, right_eye_y=?,"
                  "mouth_x=?, mouth_y=? "
                  "where user_id=?");
    query.addBindValue(coordinate.mouth.x());
    query.addBindValue(coordinate.mouth.y());
    query.addBindValue(coordinate.leftEye.x());
    query.addBindValue(coordinate.leftEye.y());
    query.addBindValue(coordinate.rightEye.x());
    query.addBindValue(coordinate.rightEye.y());
    query.addBindValue(Session::UserId);
    if (!query.exec())
        throw QString("updateCoordinate:")+query.lastError().text();
    qDebug() << "Update coordinate succeed.";
    qDebug() << query.numRowsAffected() << "rows affected";
}

///
/// \brief CDatabase::getDB return database object.
/// \return Database object.
///
void CDatabase::openDB()
{
    if (m_db.isOpen())
        throw QString("Database has already been opened by other function.");

    if (!m_db.open())
        throw QString("Opening database failed, DB cannot use\n" + m_db.lastError().text());
}
