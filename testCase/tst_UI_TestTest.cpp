#include <QString>
#include <QtTest>
#include <QDebug>


#include "cdatabase.h"
#include "DAO.h"
#include "MySession.h"
/**
   testCase :   test class DAO

*/

class UI_TestTest : public QObject
{
    Q_OBJECT

public:
    UI_TestTest();

private Q_SLOTS:
    void initTestCase();

    void cleanupTestCase();

    void testCon();

    void testUserInsert();

    void testUserQuery();

    void testLogInsert();

    void testLogQuery();
};

UI_TestTest::UI_TestTest()
{
}

void UI_TestTest::initTestCase()
{
    User user(1,"jack");
    QCOMPARE(DAO::query(user.username),true);
    Session::user =user;
}

void UI_TestTest::cleanupTestCase()
{

}

void UI_TestTest::testCon()
{

    QSqlDatabase db =CDatabase::getDB();
    QCOMPARE(db.isOpen(),true);

}

void UI_TestTest::testUserInsert()
{
    User user(1,"jack");
    DAO::insert(user);

}

void UI_TestTest::testUserQuery()
{
    QCOMPARE(DAO::query("jack") ,true);

}

void UI_TestTest::testLogInsert()
{
    Log log;
    log.date =QDate::currentDate();
    log.start_t =QTime::currentTime();
    log.end_t =QTime::currentTime().addSecs(5);
    log.face_type ="hahaha";
    log.sit_type ="heheheh";
    log.user =Session::user;
    DAO::insert(log);
}

void UI_TestTest::testLogQuery()
{
    QDate date =QDate::currentDate();
     QList<Log> logs =DAO::query(date,Session::user);
     QCOMPARE(logs.count(),2);
}

QTEST_APPLESS_MAIN(UI_TestTest)

#include "tst_UI_TestTest.moc"
