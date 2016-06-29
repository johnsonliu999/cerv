#include "logdialog.h"
#include "ui_logdialog.h"

#include "DAO.h"
//#include "User.h"
#include "MySession.h"
#include <QDebug>
#include "registerdialog.h"

#include "cdatabase.h"

#include <QMessageBox>

LogDialog::LogDialog(QWidget *parent) :
    QDialog(parent),
    mp_db(new CDatabase("login", DBParams("QMYSQL", "localhost", "neck", "root", "qq452977491", 3306))),
    ui(new Ui::LogDialog)
{
    ui->setupUi(this);
}

LogDialog::~LogDialog()
{
    QSqlDatabase::removeDatabase("login");
    delete ui;
}

void LogDialog::on_buttonBox_clicked(QAbstractButton *button)
{

    if (ui->buttonBox->button(QDialogButtonBox::Ok) == (QPushButton*)button)
    {
        try{
            mp_db->openDB();
        } catch(const QString& e)
        {
            QMessageBox::information(this, "getDB", e, QMessageBox::Ok);
            return ;
        }

        try{    
            Session::UserId = mp_db->selectUserId(ui->nameLineEdit->text());
            mp_db->closeDB();
        }catch(const QString & e)
        {
            mp_db->closeDB();
            QMessageBox::information(this, "selectUserId", e, QMessageBox::Ok);
            return;
        }

        Session::Username = ui->nameLineEdit->text();
        accept();
    }
    else if(ui->buttonBox->button(QDialogButtonBox::Cancel) == (QPushButton*)button)
    {
        reject();
    }
}

void LogDialog::on_newUserLabel_linkActivated(const QString &link)
{
    registerDialog r;
    r.show();
    if (r.exec() == QDialog::Accepted)
    {
        QMessageBox::information(this, "Register succeed", "Congratulations, new user!", QMessageBox::Yes);
    }
}
