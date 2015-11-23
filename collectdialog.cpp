#include "collectdialog.h"
#include "ui_collectdialog.h"
#include "cpredictor.h"
#include <QMessageBox>
#include <QDebug>
#include <QThread>

collectDialog::collectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::collectDialog)
{
    ui->setupUi(this);
    CPredictor* predictor = CPredictor::getPredictor();

    connect(predictor, &CPredictor::percentChanged, this, &collectDialog::setPercent, Qt::QueuedConnection);
    connect(predictor, &CPredictor::critial, this, &collectDialog::presentCritical, Qt::BlockingQueuedConnection);
    connect(predictor, &CPredictor::information, this, &collectDialog::presentInformation, Qt::BlockingQueuedConnection);
    connect(this, &collectDialog::startTrain, predictor, &CPredictor::trainData, Qt::QueuedConnection);
    connect(this, &collectDialog::tryLoadModel, predictor, &CPredictor::tryLoadModel, Qt::QueuedConnection);
}


collectDialog::~collectDialog()
{
    delete ui;
}

void collectDialog::on_buttonStart_clicked()
{
    CPredictor* predictor = CPredictor::getPredictor();
    predictor->moveToThread(&predictor->collectThread);
    predictor->collectThread.start();
    ui->buttonStart->setEnabled(false);
    int res = QMessageBox::question(this, "Choice", "Would you like to load the train model?", QMessageBox::Yes, QMessageBox::No);
    if (res == QMessageBox::Yes)
    {
        emit tryLoadModel();
//        ui->progressBar->setMinimum(0);
//        ui->progressBar->setMaximum(0);
    }
    else if (res == QMessageBox::No)
    {

        emit startTrain();
    }
}

void collectDialog::setPercent(int percent)
{
    ui->progressBar->setValue(percent);
}

void collectDialog::presentInformation(const QString title, const QString content)
{
    QMessageBox::information(this, title, content, QMessageBox::Ok);
}

void collectDialog::presentCritical(const QString title, const QString content)
{
    QMessageBox::critical(this, title, content, QMessageBox::Ok);
}


