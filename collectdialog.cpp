#include "collectdialog.h"
#include "ui_collectdialog.h"
#include "cpredictor.h"
#include <QMessageBox>
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
    CPredictor* predictor = CPredictor::getPredictor();
    try
    {
        for (int i = 0; i < CPredictor::nSitTypeNumber; i++)
        {
            QMessageBox::information(this, "Collection information", "Going to collect " + CPredictor::getSitString(i) + " data, please keep on", QMessageBox::Ok);
            predictor->CollectDataFromSerialPort((CPredictor::eSitType)i);
        }
    }catch (const QString & e)
    {
        QMessageBox::critical(this, "Collect error", e, QMessageBox::Ok);
    }

}
