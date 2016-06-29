#include "trainfacewidget.h"
#include "ui_trainfacewidget.h"

#include "cfaceclassfier.h"
#include <QDebug>
#include <QMessageBox>

TrainFaceWidget::TrainFaceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrainFaceWidget),
    mp_classifier(new CFaceClassfier),
    mp_reader(new CCameraReader),
    mp_procThread(new QThread)
{
    ui->setupUi(this);

    connect(this, &TrainFaceWidget::startTrain, mp_classifier, &CFaceClassfier::train);
    connect(mp_classifier, &CFaceClassfier::updateDisp, this, &TrainFaceWidget::updateDisp);
    connect(mp_classifier, &CFaceClassfier::info, this, &TrainFaceWidget::info);

    connect(this, &TrainFaceWidget::startDispCamera, mp_reader, &CCameraReader::startDispCamera);
    connect(this, &TrainFaceWidget::stopDispCamera, mp_reader, &CCameraReader::stopDispCamera);
    connect(mp_reader, &CCameraReader::updateCameraDisp, this, &TrainFaceWidget::updateCameraDisp);

    mp_reader->moveToThread(mp_procThread);
    mp_classifier->moveToThread(mp_procThread);
    mp_procThread->start();
    emit startDispCamera();
}

TrainFaceWidget::~TrainFaceWidget()
{
    mp_procThread->quit();
    mp_procThread->wait();
    delete ui;
}

void TrainFaceWidget::info(const QString &title, const QString &text)
{
    QMessageBox::information(this, title, text, QMessageBox::Ok);
}

void TrainFaceWidget::updateCameraDisp(const QImage &img)
{
    qDebug() << "TrainFaceWidget::updateCameraDisp called.";
    ui->CameraDisp->setPixmap(QPixmap::fromImage(img));
    //    ui->CameraDisp->show();
}

void TrainFaceWidget::updateDisp(const QImage &img, const QString &remainTime)
{
    ui->CameraDisp->setPixmap(QPixmap::fromImage(img));
    ui->remainTime->setText(remainTime);
}

void TrainFaceWidget::on_startBtn_clicked()
{
    emit stopDispCamera();
    emit startTrain();
    ui->startBtn->setEnabled(false);
}
