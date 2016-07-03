#include "ReportWindow.h"
#include "logger.h"
#include "cfaceclassfier.h"
#include "cpredictor.h"
#include "loadder.h"

#include "ui_ReportWindow.h"
#include <QStandardItemModel>
#include <QMessageBox>

ReportWindow::ReportWindow(QWidget *parent, const QDate &date) :
    QMainWindow(parent),
    mp_model(new QStandardItemModel(this)),
    m_date(date),
    ui(new Ui::ReportWindow)
{
    ui->setupUi(this);

    setupTable();
}

ReportWindow::~ReportWindow()
{

    delete ui;
}

void ReportWindow::setupTable()
{
    //set model
    mp_model->setColumnCount(3);

    mp_model->setHeaderData(0,Qt::Horizontal,"Time");
    mp_model->setHeaderData(1,Qt::Horizontal,"Face Type");
    mp_model->setHeaderData(2,Qt::Horizontal,"Sit Type");



    //set tableView
    ui->tableView->setModel(mp_model);
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

    mp_loadder = new Loadder;
    qRegisterMetaType<QList<Log> >("QList<Log>");
    connect(this, &ReportWindow::loadLog, mp_loadder, &Loadder::loadLog);
    connect(mp_loadder, &Loadder::updateLogTable, this, &ReportWindow::updateLogTable);
    connect(mp_loadder, &Loadder::finishedLoad, this, &ReportWindow::deleteLoadder);
    connect(mp_loadder, &Loadder::info, this, &ReportWindow::info);

    emit loadLog(m_date);
}

void ReportWindow::updateLogTable(const QList<Log> &logList)
{
    qDebug() << "Called updateLogTable";
    qDebug() << logList.size();
    for (int i = 0; i < logList.size(); i++)
    {
        Log log = logList[i];
        mp_model->setItem(i, 0, new QStandardItem(log.time.toString("hh:mm:ss")));
        mp_model->setItem(i, 1, new QStandardItem(CFaceClassfier::Enum2String((Face::FaceType)log.faceType)));
        mp_model->setItem(i, 2, new QStandardItem(CPredictor::Enum2String((Sit::SitType)log.sitType)));
    }
}

void ReportWindow::info(const QString &title, const QString &text)
{
    QMessageBox::information(this, title, text, QMessageBox::Ok);
}

void ReportWindow::deleteLoadder()
{
    delete mp_loadder;
}
