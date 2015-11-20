#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "DAO.h"
#include <QMessageBox>
#include <QDebug>

registerDialog::registerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::registerDialog)
{
    ui->setupUi(this);
}

registerDialog::~registerDialog()
{
    delete ui;
}

void registerDialog::on_buttonBox_accepted()
{
    try
    {
        if(DAO::insert(User(ui->registerLineEdit->text())))
            accept();
    }catch (const QString & e)
    {
        QMessageBox::critical(this, "Database errror", e, QMessageBox::Yes);
    }
}

void registerDialog::on_buttonBox_rejected()
{
    reject();
}
