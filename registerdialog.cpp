#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "DAO.h"
#include <QMessageBox>
#include <QDebug>

#include "cdatabase.h"
registerDialog::registerDialog(QWidget *parent) :
    QDialog(parent),
    mp_db(new CDatabase("register", DBParams("QMYSQL", "localhost", "neck", "root", "qq452977491", 3306))),
    ui(new Ui::registerDialog)
{
    ui->setupUi(this);
}

registerDialog::~registerDialog()
{
    QSqlDatabase::removeDatabase("registor");
    delete ui;
}

void registerDialog::on_buttonBox_accepted()
{
    try{
        mp_db->openDB();
    } catch(const QString &e)
    {
        QMessageBox::information(this, "Database errror", e, QMessageBox::Ok);
        return ;
    }

    try
    {
        mp_db->insertUserName(ui->registerLineEdit->text());
        accept();
        mp_db->closeDB();
    }catch (const QString & e)
    {
        mp_db->closeDB();
        QMessageBox::information(this, "Database errror", e, QMessageBox::Ok);
    }

}

void registerDialog::on_buttonBox_rejected()
{
    reject();
}
