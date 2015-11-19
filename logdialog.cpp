#include "logdialog.h"
#include "ui_logdialog.h"
#include <QMessageBox>
#include "DAO.h"
#include "User.h"
#include "MySession.h"

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
            if (DAO::query(ui->nameLineEdit->text()))
            {
                Session::user.userid = 1; // to be decided
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
    }
}
