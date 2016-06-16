#include "collectdialog.h"
#include "ui_collectdialog.h"
#include "cpredictor.h"
#include <QMessageBox>
#include <QDebug>
#include <QThread>

collectDialog::collectDialog(const QString &portName, const bool b_replace, QWidget *parent) :
    QDialog(parent),portName(portName), b_replace(b_replace),
    ui(new Ui::collectDialog)
{
    ui->setupUi(this);
    CPredictor* predictor = CPredictor::getPredictor();

    connect(predictor, &CPredictor::percentChanged, this, &collectDialog::setPercent);
    connect(predictor, &CPredictor::information, this, &collectDialog::presentInformation, Qt::BlockingQueuedConnection);
    connect(predictor, &CPredictor::finishedTrain, this, &collectDialog::accept);

    connect(this, &collectDialog::startTrain, predictor, &CPredictor::trainData);
    connect(this, &collectDialog::tryLoadModel, predictor, &CPredictor::tryLoadModel);
}


collectDialog::~collectDialog()
{
    collectThread.quit();
    collectThread.wait();
    delete ui;
}

#include "cserialreader.h"
void collectDialog::on_buttonStart_clicked()
{
    CPredictor* p_predictor = CPredictor::getPredictor();
    p_predictor->moveToThread(&collectThread);
    collectThread.start();

    ui->buttonStart->setEnabled(false);
    emit startTrain(portName, b_replace);
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


