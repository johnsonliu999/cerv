#include "logdialog.h"
#include "ui_logdialog.h"
#include <QMessageBox>
#include "DAO.h"
#include "User.h"
#include "MySession.h"
#include <QDebug>
#include "registerdialog.h"

LogDialog::LogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogDialog)
{
    ui->setupUi(this);
}

LogDialog::~LogDialog()
{
    delete ui;
}

void LogDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if (ui->buttonBox->button(QDialogButtonBox::Ok) == (QPushButton*)button)
    {
        try{
            int id;
            if (DAO::query(ui->nameLineEdit->text(), id))
            {
                Session::user.userid = id; // to be decided
                Session::user.username = ui->nameLineEdit->text();
                accept();
            }
            else
                QMessageBox::warning(this, "Warning", "User does not exist", QMessageBox::Yes);
        }catch(const QString & e)
        {
            QMessageBox::critical(this, "Database connect error", e, QMessageBox::Yes);
        }
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
