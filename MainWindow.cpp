#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QMessageBox>
#include "ReportWindow.h"
#include <QTime>

//data
#include "Log.h"
#include "DAO.h"
#include "MySession.h"
//sit
#include "SitLogic.h"
//face
#include "capture.h"
#include "FaceLogic.h"
#include "collectdialog.h"




void MainWindow::seatProcess()
{

    QString seatResultDisplayString = "";    

    SitLogic::readOnce();
    for(auto res : SitLogic::getSitType())
    {
        seatResultDisplayString=SitLogic::fetchJudgedMessage(res)+"\n";
    }

    ui->seatInfoEdit->setPlainText(seatResultDisplayString);

}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),timer(new QTimer)
{
    ui->setupUi(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timing()));

}

MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
}



void MainWindow::shakeFrm()
{

}



bool first =true;
int face_duration =0;
FacePostureType previous;
QTime start_t;
QTime end_t;
void MainWindow::timing()
{


    ui->label->setPixmap(QPixmap::fromImage(detect()));
    seatProcess();

    if(first)
    {
        previous =FaceLogic::getRtType();
        first =false;
        start_t =QTime::currentTime();
    }
    if(FaceLogic::getRtType() == previous)
    {
        face_duration++;
    }
    else
    {
        //����Ϊ�ύ����
        if(face_duration>=5)
        {
            end_t =QTime::currentTime();

            Log log;
            log.date =QDate::currentDate();
            log.start_t =start_t;
            log.end_t =end_t;
            log.face_type =FaceLogic::getRtType();
            log.sit_type =SitLogic::getAverageType();
            log.user =Session::user;

            DAO::insert(log);

        }

        previous =FaceLogic::getRtType();

        start_t =QTime::currentTime();
    }

    //���ͬһ���Ƴ�����Σ���������Ӧ����
    if(face_duration>=5)
    {
        switch(FaceLogic::getRtType())
        {
           case Profile:
              ui->statusBar->showMessage("you are tilting your face too long!", 1000);
              break;

           case NO_FACE:
              ui->statusBar->showMessage("you have left the computer...", 1000);
              shakeFrm();
           //�����Ĵ����ټ�
           case Normal:
              if(face_duration>=60)
              {
                ui->statusBar->showMessage("hold on,you have good posture for a minute!",2000);
              }
                break;
           default:
                break;
        }
    }
    ui->plainTextEdit->setPlainText(FaceLogic::fetchJudgedMessage());




}

void MainWindow::on_MainWindow_destroyed()
{
     closeCamera();
}

void MainWindow::on_calendarWidget_clicked(const QDate &date)
{

    QList<Log> logs =DAO::query(const_cast<QDate&>(date),Session::user);

    ReportWindow* w =new ReportWindow(logs,this);
    w->show();
}

void MainWindow::on_pushButton_4_clicked()
{

    if(ui->pushButton_4->text()=="Start")
    {
        if(!isAllowCameraOpen())
        {
            QMessageBox::StandardButton bt =QMessageBox::information(this,"open Camera",tr("this application requests to open your camera,do you agree?"),QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            if(bt==QMessageBox::No)
                return;
            allowCameraOpen();
        }
        try
        {
            openCamera();
            qDebug() << "Start seat init";
            SitLogic::init();
            qDebug() << "Finish seat init";
        }catch(const char* msg)
        {
            QMessageBox::information(this,"error",tr(msg));
                return;
        }

        //��ʱ����
        qDebug() << "Start timer";
        timer->start(1000);
        ui->pushButton_4->setText("Stop");
    }
    else
    {
        qDebug() << "Stop timer";
        timer->stop();
        ui->pushButton_4->setText("Start");
    }
}

void MainWindow::on_actionTrain_triggered()
{
    collectDialog c;
    c.show();
    c.exec();
}
