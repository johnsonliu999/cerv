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

#include <QList>
#include <QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),timer(new QTimer)
{
    ui->setupUi(this);

    QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();
    ui->COMComboBox->setCurrentText("None");
    for (int i = 0; i < portList.length(); i++)
        ui->COMComboBox->addItem(portList[i].portName());

    connect(timer,SIGNAL(timeout()),this,SLOT(timing()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
}


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
        //以下为提交报告
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

    //如果同一姿势超过五次，则按姿势相应处理
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
           //其他的待会再加
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
    if ("" == ui->COMComboBox->currentText())
    {
        QMessageBox::information(this, "Error", "No COM selected.", QMessageBox::Ok);
        return;
    }

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
        }catch(const QString & e)
        {
            QMessageBox::information(this,"error",e);
            return;
        }

        CSerialReader* p_reader = CSerialReader::getReader();
        p_reader->OpenSerial(ui->COMComboBox->currentText());

        CPredictor* p_predictor = CPredictor::getPredictor();
        Predictor p;
        try{
            p = DAO::query(Session::user);
        }catch (const QString& e)
        {
            QMessageBox::information(this, "Model error", e);
            return;
        }

        QFile f("temp.xml");
        f.open(QIODevice::WriteOnly);
        f.write(p.xml);
        f.close();
        p_predictor->mp_trees = Algorithm::load<RTrees>("temp.xml");

        qDebug() << (p_predictor->mp_trees->isTrained() ? "Trained" : "Untrained");

        //定时截屏
        qDebug() << "Start timer";
        timer->start(1000);
        ui->pushButton_4->setText("Stop");
    }
    else
    {
        qDebug() << "Stop timer";
        timer->stop();

        CSerialReader* p_reader = CSerialReader::getReader();
        p_reader->CloseSerial();

        ui->pushButton_4->setText("Start");
    }
}

void MainWindow::on_actionTrain_triggered()
{
    if ("" == ui->COMComboBox->currentText())
    {
        QMessageBox::information(this, "Error", "No COM selected.", QMessageBox::Ok);
        return;
    }

    collectDialog c(ui->COMComboBox->currentText(), this);
    c.exec();
}
