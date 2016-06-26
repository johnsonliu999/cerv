#include "collectdialog.h"
#include "ui_collectdialog.h"
#include "cpredictor.h"
#include <QMessageBox>
#include <QDebug>
#include <QThread>

///
/// \brief collectDialog::collectDialog
/// contruct collecting dialog and make connection.
///
/// \param portName
/// \param b_replace
/// \param parent
///
collectDialog::collectDialog(const QString &portName, const bool b_replace, QWidget *parent) :
    QDialog(parent),
    portName(portName),
    mb_replace(b_replace),
    mp_predictor(new CPredictor),
    ui(new Ui::collectDialog)
{
    ui->setupUi(this);

    connect(mp_predictor, &CPredictor::percentChanged, this, &collectDialog::setPercent);
    connect(mp_predictor, &CPredictor::information, this, &collectDialog::info, Qt::BlockingQueuedConnection);
    connect(mp_predictor, &CPredictor::finishedTrain, this, &collectDialog::accept);

    connect(this, &collectDialog::startTrain, mp_predictor, &CPredictor::trainData);
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
    mp_predictor->moveToThread(&collectThread);
    collectThread.start();

    ui->buttonStart->setEnabled(false);
    emit startTrain(portName, mb_replace);
}

void collectDialog::setPercent(int percent)
{
    ui->progressBar->setValue(percent);
}

void collectDialog::info(const QString title, const QString content)
{
    QMessageBox::information(this, title, content, QMessageBox::Ok);
}


