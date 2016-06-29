#if NO
#include "ReportWindow.h"
#include "ui_ReportWindow.h"
#include <QStandardItemModel>

ReportWindow::ReportWindow( QList<Log>& reportSet,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ReportWindow)
{
    ui->setupUi(this);

    setTable(reportSet);

}

ReportWindow::~ReportWindow()
{
    delete ui;
}

void ReportWindow::setTable(QList<Log>& reportSet)
{
    QStandardItemModel  *model = new QStandardItemModel();

    //set model
    model->setColumnCount(6);
    model->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("Index"));
    model->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("Face Type"));
    model->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("Sit Type"));
    model->setHeaderData(3,Qt::Horizontal,QString::fromLocal8Bit("Start_Time"));
    model->setHeaderData(4,Qt::Horizontal,QString::fromLocal8Bit("End_Time"));
    model->setHeaderData(5,Qt::Horizontal,QString::fromLocal8Bit("Duration(sec)"));

    //set tableView
    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode(4,QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode(5,QHeaderView::Fixed);

    ui->tableView->setColumnWidth(0,101);
    ui->tableView->setColumnWidth(1,102);
    ui->tableView->setColumnWidth(2,102);
    ui->tableView->setColumnWidth(3,102);
    ui->tableView->setColumnWidth(4,102);
    ui->tableView->setColumnWidth(5,102);


    // add data
     for(int i = 0; i < reportSet.size(); i++)
     {
         Log log =reportSet[i];
         model->setItem(i,0,new QStandardItem(log.id));          
         model->item(i,0)->setForeground(QBrush(QColor(255, 0, 0)));        
         model->item(i,0)->setTextAlignment(Qt::AlignCenter);
         model->setItem(i,1,new QStandardItem(log.face_type));
         model->setItem(i,2,new QStandardItem(log.sit_type));
         model->setItem(i,3,new QStandardItem(log.start_t.toString("hh:mm:ss")));
         model->setItem(i,4,new QStandardItem(log.end_t.toString("hh:mm:ss")));
         model->setItem(i,5,new QStandardItem(log.start_t.secsTo(log.end_t)));
     }

}
#endif
