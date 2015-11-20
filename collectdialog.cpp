#include "collectdialog.h"
#include "ui_collectdialog.h"
#include "cpredictor.h"
#include <QDebug>

collectDialog::collectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::collectDialog)
{
    ui->setupUi(this);
}

collectDialog::~collectDialog()
{
    delete ui;
}

void collectDialog::on_buttonStart_clicked()
{

}
