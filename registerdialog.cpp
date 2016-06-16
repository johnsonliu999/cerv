#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "DAO.h"
#include <QMessageBox>
#include <QDebug>

#include "cdatabase.h"
registerDialog::registerDialog(QWidget *parent) :
    QDialog(parent),
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
    DBParams params;
    params.database = "QMYSQL";
    params.host = "localhost";
    params.database = "neck";
    params.user = "root";
    params.password = "qq452977491";
    params.port = 3306;

    CDatabase d("register", params);
    try
    {
        QSqlDatabase db = d.getDB();
        if(DAO::insert(db, User(ui->registerLineEdit->text())))
            accept();
        db.close();
    }catch (const QString & e)
    {
        QMessageBox::information(this, "Database errror", e, QMessageBox::Ok);
    }

}

void registerDialog::on_buttonBox_rejected()
{
    reject();
}
