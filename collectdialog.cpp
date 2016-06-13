#include "collectdialog.h"
#include "ui_collectdialog.h"
#include "cpredictor.h"
#include <QMessageBox>
#include <QDebug>
#include <QThread>

collectDialog::collectDialog(QString &portName, QWidget *parent) :
    QDialog(parent),portName(portName),
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

    int res = QMessageBox::question(this, "Choice", "Would you like to load the train model?", QMessageBox::Yes, QMessageBox::No);
    if (res == QMessageBox::Yes)
        emit tryLoadModel();
    else if (res == QMessageBox::No)
    {
        res = QMessageBox::question(this, "Question", "Wired?", QMessageBox::Yes, QMessageBox::No);
        bool wired = ( (res == QMessageBox::Yes ) ? true : false);
        emit startTrain(portName, wired);
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


