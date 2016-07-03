#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QMessageBox>
#include "ReportWindow.h"
#include <QTime>

//data
#include "MySession.h"
//sit
#include "SitLogic.h"
//face
#include "FaceLogic.h"
#include "collectdialog.h"
#include "logger.h"

#include <QList>
#include <QSerialPortInfo>

#include "connectdialog.h"
#include "trainfacewidget.h"
#include "barchartwindow.h"

MainWindow::MainWindow(bool wired, QWidget *parent) :
    QMainWindow(parent), wired(wired),
    ui(new Ui::MainWindow),
    mp_db(new CDatabase("main_window", DBParams("QMYSQL", "localhost", "neck", "root", "qq452977491", 3306))),
    mp_sitProcTimer(new QTimer(this)),
    mp_cameraTimer(new QTimer(this)),
    mp_sitLogic(new SitLogic(this)),
    mp_faceLogic(new FaceLogic(this)),
    mp_sitLogicThd(new QThread(this)),
    mp_faceLogicThd(new QThread(this))
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
    connect(mp_sitLogic, &SitLogic::updateDisp, this, &MainWindow::updateSitDisp);
    connect(mp_sitLogic, &SitLogic::info, this, &MainWindow::info);

    // face
    connect(this, &MainWindow::updateFaceRes, mp_faceLogic, &FaceLogic::updateFaceRes);
    connect(mp_faceLogic, &FaceLogic::updateDisp, this, &MainWindow::updateFaceDisp);
    connect(mp_faceLogic, &FaceLogic::info, this, &MainWindow::info);
    connect(mp_faceLogic, &FaceLogic::updateFaceDisp, this, &MainWindow::updateCameraDisp);

    /* end connection */

    mp_sitLogic->setParent(0);
    mp_sitLogic->moveToThread(mp_sitLogicThd);
    mp_sitLogicThd->start();

    mp_faceLogic->setParent(0);
    mp_faceLogic->moveToThread(mp_faceLogicThd);
    mp_faceLogicThd->start();
}

MainWindow::~MainWindow()
{
    QSqlDatabase::removeDatabase("main_win");

//    mp_faceLogicThd->quit();
//    mp_faceLogicThd->wait();

    mp_sitLogicThd->quit();
    mp_sitLogicThd->wait();
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

void MainWindow::updateCameraDisp(const QImage &img)
{
    ui->cameraDisp->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::deleteLogger()
{
    delete mp_logger;
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
}

void MainWindow::on_MainWindow_destroyed()
{
}

void MainWindow::on_calendarWidget_clicked(const QDate &date)
{
    ReportWindow* w = new ReportWindow(this, date);
    w->show();
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

        QString portName(ui->COMComboBox->currentText());
        if ("" == portName)
            QMessageBox::information(this, "startButtonClicked", "No COM selected.", QMessageBox::Ok);

        //定时截屏
        qDebug() << "Start timer";
        try {
            mp_sitLogic->start();
            mp_faceLogic->start();
        } catch(const QString &e)
        {
            QMessageBox::information(this, "start button clicked", e, QMessageBox::Ok);
            return ;
        }
        mp_cameraTimer->start(4000);
        mp_sitProcTimer->start(3000);
        ui->COMComboBox->setEnabled(false);
        ui->startButton->setText("Stop");

        // logger
        mp_logger = new Logger(mp_faceLogic, mp_sitLogic);
        connect(this, &MainWindow::startLog, mp_logger, &Logger::start);
        connect(this, &MainWindow::stopLog, mp_logger, &Logger::stop);
        connect(mp_logger, &Logger::stopped, this, &MainWindow::deleteLogger);

        emit startLog();
    }
    else
    {
        emit stopLog();
        mp_cameraTimer->stop();
        mp_sitProcTimer->stop();
        mp_sitLogic->stop();
        mp_faceLogic->stop();
        ui->COMComboBox->setEnabled(true);
        ui->startButton->setText("Start");
    }
}

void MainWindow::on_trainFace_triggered()
{
    mp_trainFaceWidget = new TrainFaceWidget;
    mp_trainFaceWidget->show();
}

void MainWindow::on_trainSit_triggered()
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

void MainWindow::on_actionUser_Name_triggered()
{
    QMessageBox::information(this, "User Name", Session::Username, QMessageBox::Ok);
}

void MainWindow::on_actionUser_Id_triggered()
{
    QMessageBox::information(this, "User ID", QString::number(Session::UserId), QMessageBox::Ok);
}

void MainWindow::on_actionBar_Chart_triggered()
{
    BarChartWindow *p_barChartWindow = new BarChartWindow(this);
    p_barChartWindow->show();
}
