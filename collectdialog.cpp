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

#include "cserialreader.h"
void collectDialog::on_buttonStart_clicked()
{
    CPredictor* p_predictor = CPredictor::getPredictor();
    p_predictor->moveToThread(&p_predictor->collectThread);
    p_predictor->collectThread.start();

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


