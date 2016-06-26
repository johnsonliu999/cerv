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

#include "connectdialog.h"

MainWindow::MainWindow(bool wired, QWidget *parent) :
    QMainWindow(parent), wired(wired),
    ui(new Ui::MainWindow),mp_timer(new QTimer),
    mp_sitLogic(new SitLogic(Session::user)),
    mp_sitLogicThd(new QThread)
{
    ui->setupUi(this);

    if (wired) ui->connectButton->~QPushButton();

    QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();

    for (int i = 0; i < portList.length(); i++)
        ui->COMComboBox->addItem(portList[i].portName());

    // begin connection
    connect(mp_timer,SIGNAL(timeout()),this,SLOT(timing()));

    connect(this, &MainWindow::updateSitData, mp_sitLogic, &SitLogic::updateSitData);
    connect(this, &MainWindow::updateModel, mp_sitLogic, &SitLogic::updateModel);
    connect(mp_sitLogic, &SitLogic::updatePlainText, this, &MainWindow::updatePlainText);
    connect(mp_sitLogic, &SitLogic::info, this, &MainWindow::info);
    // end connection

    // begin DB initialization
    DBParams params;
    params.database = "QMYSQL";
    params.host = "localhost";
    params.database = "neck";
    params.user = "root";
    params.password = "qq452977491";
    params.port = 3306;
    m_db = CDatabase("main_win", params);
    // end DB initialization

    mp_sitLogic->moveToThread(mp_sitLogicThd);
    mp_sitLogicThd->start();
}

MainWindow::~MainWindow()
{
    QSqlDatabase::removeDatabase("main_win");
    delete ui;
    delete mp_timer;
}


void MainWindow::sitProc()
{
    qDebug() << "emit updateSitData";
    emit updateSitData(ui->COMComboBox->currentText());
}

void MainWindow::shakeFrm()
{

}

// slots
void MainWindow::updatePlainText(const QString text)
{
    ui->seatInfoEdit->setPlainText(text);
}

void MainWindow::info(const QString title, const QString text)
{
    QMessageBox::information(this, title, text, QMessageBox::Ok);
}

// end of slots

bool first =true;
int face_duration =0;
FacePostureType previous;
QTime start_t;
QTime end_t;

void MainWindow::timing()
{
    ui->label->setPixmap(QPixmap::fromImage(detect()));

    sitProc();

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
            log.sit_type = mp_sitLogic->getRecentRes();
            log.user =Session::user;

            try{
                QSqlDatabase db = m_db.getDB();
                DAO::insert(db, log);
                db.close();
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
}

void MainWindow::on_MainWindow_destroyed()
{
     closeCamera();
}

void MainWindow::on_calendarWidget_clicked(const QDate &date)
{
    try{
        QSqlDatabase db = m_db.getDB();
        QList<Log> logs =DAO::query(db, const_cast<QDate&>(date),Session::user);
        db.close();
        ReportWindow* w =new ReportWindow(logs,this);
        w->show();
    }catch (const QString& e)
    {
        QMessageBox::information(this, "Error", e, QMessageBox::Ok);
    }
}

void MainWindow::on_actionTrain_triggered()
{
    QString portName(ui->COMComboBox->currentText());
    bool b_replace = false;

    try{
        if ("" == portName) throw QString("No selected COM");
        QSqlDatabase db = m_db.getDB();
        Predictor p = DAO::query(db, Session::user);
        db.close();
        if (p.id)
        {
            int res = QMessageBox::question(this, "Predictor model detected.", "Would you like to replace the existed model?", QMessageBox::Yes, QMessageBox::No);
            if (res == QMessageBox::No) return;
            else b_replace = true;
        }
    } catch (const QString& e)
    {
        QMessageBox::information(this, "train_triggered", e, QMessageBox::Ok);
        return ;
    }

    ui->COMComboBox->setEnabled(false);

    collectDialog c(portName, b_replace);
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
            openCamera();
            if ("" == portName) throw QString("No selected COM.");

//            Predictor p;
//            QSqlDatabase db = m_db.getDB();
//            p = DAO::query(db, Session::user);
//            db.close();

            emit updateModel();
        }catch (const QString& e)
        {
            QMessageBox::information(this, "Model error", e);
            return;
        }

        //qDebug() << (p_predictor->mp_trees->isTrained() ? "Trained" : "Untrained");

        //定时截屏
        qDebug() << "Start timer";
        mp_timer->start(1000);
        ui->startButton->setText("Stop");
    }
    else
    {
        qDebug() << "Stop timer";
        mp_timer->stop();
        ui->COMComboBox->setEnabled(true);
        ui->startButton->setText("Start");
    }
}
