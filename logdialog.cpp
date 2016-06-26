#include "logdialog.h"
#include "ui_logdialog.h"
#include <QMessageBox>
#include "DAO.h"
#include "User.h"
#include "MySession.h"
#include <QDebug>
#include "registerdialog.h"

#include "cdatabase.h"
LogDialog::LogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogDialog)
{
    ui->setupUi(this);

    DBParams params;
    params.database = "QMYSQL";
    params.host = "localhost";
    params.database = "neck";
    params.user = "root";
    params.password = "qq452977491";
    params.port = 3306;

    mp_db = new CDatabase("login", params);
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
        QSqlDatabase db;
        try{
            db = mp_db->getDB();
        } catch(const QString& e)
        {
            QMessageBox::information(this, "getDB", e, QMessageBox::Ok);
            return ;
        }

        int id;
        try{    
            if (DAO::query(db, ui->nameLineEdit->text(), id))
            {
                Session::user.userid = id; // to be decided
                Session::user.username = ui->nameLineEdit->text();
            }
            else throw QString("User not exist");
        }catch(const QString & e)
        {
            db.close();
            QMessageBox::information(this, "Database connect error", e, QMessageBox::Yes);
        }

        db.close();
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
