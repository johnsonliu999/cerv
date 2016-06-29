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
#include "FaceLogic.h"
#include "collectdialog.h"

#include <QList>
#include <QSerialPortInfo>

#include "connectdialog.h"

MainWindow::MainWindow(bool wired, QWidget *parent) :
    QMainWindow(parent), wired(wired),
    ui(new Ui::MainWindow),
    mp_db(new CDatabase("main_window", DBParams("QMYSQL", "localhost", "neck", "root", "qq452977491", 3306))),
    mp_sitProcTimer(new QTimer),
    mp_cameraTimer(new QTimer),
    mp_sitLogic(new SitLogic),
    mp_faceLogic(new FaceLogic),
    mp_sitLogicThd(new QThread),
    mp_faceLogicThd(new QThread)
{
    ui->setupUi(this);

    if (wired) ui->connectButton->~QPushButton();

    QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();

    for (int i = 0; i < portList.length(); i++)
        ui->COMComboBox->addItem(portList[i].portName());

    /* begin connection */
    connect(mp_sitProcTimer,&QTimer::timeout,this,&MainWindow::sitTask);
    connect(mp_cameraTimer, &QTimer::timeout, this, &MainWindow::cameraTask);

    // sit
    connect(this, &MainWindow::updateSitRes, mp_sitLogic, &SitLogic::updateSitRes);
    connect(this, &MainWindow::updateModel, mp_sitLogic, &SitLogic::updateModel);
    connect(mp_sitLogic, &SitLogic::updateDisp, this, &MainWindow::updateSitDisp);
    connect(mp_sitLogic, &SitLogic::info, this, &MainWindow::info);

    // face
    connect(this, &MainWindow::updateFaceRes, mp_faceLogic, &FaceLogic::updateFaceRes);
    connect(mp_faceLogic, &FaceLogic::updateDisp, this, &MainWindow::updateFaceDisp);
    connect(mp_faceLogic, &FaceLogic::info, this, &MainWindow::info);
    /* end connection */

    mp_sitLogic->moveToThread(mp_sitLogicThd);
    mp_sitLogicThd->start();

    mp_faceLogic->moveToThread(mp_faceLogicThd);
    mp_faceLogicThd->start();
}

MainWindow::~MainWindow()
{
    QSqlDatabase::removeDatabase("main_win");

    mp_faceLogicThd->quit();
    mp_faceLogicThd->wait();

    mp_sitLogicThd->quit();
    mp_faceLogicThd->wait();
}

void MainWindow::shakeFrm()
{

}

/* slots */
void MainWindow::updateSitDisp(const QString text)
{
    ui->sitDisp->setText(text);
}

void MainWindow::updateFaceDisp(const QString text)
{
    ui->faceDisp->setText(text);
}

void MainWindow::info(const QString title, const QString text)
{
    QMessageBox::information(this, title, text, QMessageBox::Ok);
}

void MainWindow::sitTask()
{
    emit updateSitRes(ui->COMComboBox->currentText());
}

void MainWindow::cameraTask()
{
    emit updateFaceRes();
    /*
    if(first)
    {
        previous =FaceLogic::getRtType();
        first =false;
        start_t =QTime::currentTime();
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
            log.sit_type = mp_sitLogic->getRecentRes();
            log.user =Session::user;

            try{
                m_db.openDB();
                DAO::insert(db, log);
                m_db.closeDB();
            } catch (const QString& e)
            {
                QMessageBox::information(this, "Error", e, QMessageBox::Ok);
            }
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
    */
}

void MainWindow::on_MainWindow_destroyed()
{
}

void MainWindow::on_calendarWidget_clicked(const QDate &date)
{
#if NO
    try{
        mp_db->openDB();
        QList<Log> logs =DAO::selectCoordinate(db, const_cast<QDate&>(date),Session::user);
        mp_db->closeDB();
        ReportWindow* w =new ReportWindow(logs,this);
        w->show();
    }catch (const QString& e)
    {
        QMessageBox::information(this, "Error", e, QMessageBox::Ok);
    }
#endif
}

void MainWindow::on_actionTrain_triggered()
{
    try{
        mp_db->openDB();
    } catch(const QString &e)
    {
        QMessageBox::information(this, "train_triggered", e, QMessageBox::Ok);
        return ;
    }

    bool b_exist = true;
    try{
        mp_db->selectXml(); // check if model exists
        mp_db->closeDB();
    } catch (const QString& e)
    {
        mp_db->closeDB();
        if (!e.contains("no record", Qt::CaseInsensitive))
        {
            QMessageBox::information(this, "train_triggered", e, QMessageBox::Ok);
            return ;
        }
        else b_exist = false;
    }
    if (b_exist)
    {
        int res = QMessageBox::question(this, "Predictor model detected.",
                              "Would you like to replace the existed model?",
                              QMessageBox::Yes, QMessageBox::No);
        if (res == QMessageBox::No) return;
    }

    QString portName(ui->COMComboBox->currentText());
    if (portName == "")
    {
        QMessageBox::information(this, "No COM", "No COM is selected.", QMessageBox::Ok);
        return;
    }

    ui->COMComboBox->setEnabled(false);
    collectDialog c(portName, b_exist);
    c.exec();
    ui->COMComboBox->setEnabled(true);
}

void MainWindow::on_connectButton_clicked()
{
    QString portName(ui->COMComboBox->currentText());

    // open serial port
    QList<QString> devList;
    try{
        if ("" == portName) throw QString("No selected COM.");
        CSerialReader reader(portName);
        devList = reader.findDev();
        if (devList.empty())
            throw QString("No device found");
    } catch (const QString& e)
    {
        QMessageBox::information(this, "Error", e, QMessageBox::Ok);
        return;
    }

    connectDialog c(portName, devList);
    c.exec();
}

void MainWindow::on_startButton_clicked()
{
    if(ui->startButton->text()=="Start")
    {
        ui->COMComboBox->setEnabled(false);
        QString portName(ui->COMComboBox->currentText());
        try
        {
            if ("" == portName) throw QString("No selected COM.");
            emit updateModel();
        }catch (const QString& e)
        {
            QMessageBox::information(this, "Model error", e);
            return;
        }

        //定时截屏
        qDebug() << "Start timer";
        mp_cameraTimer->start(1000);
        mp_sitProcTimer->start(1000);
        ui->startButton->setText("Stop");
    }
    else
    {
        qDebug() << "Stop timer";
        mp_cameraTimer->stop();
        mp_sitProcTimer->stop();
        ui->COMComboBox->setEnabled(true);
        ui->startButton->setText("Start");
    }
}
