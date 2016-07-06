#include "barchartwindow.h"
#include "ui_barchartwindow.h"

#include "loadder.h"

#include "qcustomplot.h"

BarChartWindow::BarChartWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BarChartWindow),
//    mp_bars(new QCPBars(ui->widget->xAxis, ui->widget->yAxis)),
    mp_loadder(new Loadder)
{
    ui->setupUi(this);

    mp_bars = new QCPBars(ui->widget->xAxis, ui->widget->yAxis);

    qRegisterMetaType<QVector<double> >("QVector<double>");
    connect(this, SIGNAL(loadBarChartData(QDate,QDate)), mp_loadder, SLOT(loadLog(QDate,QDate)));
    connect(mp_loadder, &Loadder::updateBarChart, this, &BarChartWindow::updateBarChart);
    connect(mp_loadder, &Loadder::infoBarChart, this, &BarChartWindow::info);
    connect(mp_loadder, &Loadder::updateShow, this, &BarChartWindow::updateShow);

    initalizeBarChart();
}

BarChartWindow::~BarChartWindow()
{
    delete mp_bars;
    delete mp_loadder;
    delete ui;
}

void BarChartWindow::initalizeBarChart()
{
    ui->widget->xAxis->setAutoTicks(false);
    ui->widget->xAxis->setAutoTickLabels(false);
    ui->widget->xAxis->setAutoTickStep(false);
    ui->widget->addPlottable(mp_bars);

    QDate today = QDate::currentDate();
    emit loadBarChartData(today.addDays(-7),today);
}

void BarChartWindow::updateBarChart(const QVector<double> &values)
{
    qDebug() << values;
    QVector<double> index;
    index << 0 << 1 << 2 << 3 << 4 << 5;
    mp_bars->setData(index, values);
    ui->widget->rescaleAxes();

    double width = ui->widget->xAxis->range().upper - ui->widget->xAxis->range().lower;
    double step = mp_bars->width()+(width-mp_bars->width()*5)/4;

    qDebug() << "upper" << ui->widget->xAxis->range().upper;
    qDebug() << "lower" << ui->widget->xAxis->range().lower;
    qDebug() << "step" << step;


    QVector<double> coor;
    for (int i = 0; i < 6; i++)
        coor.append(ui->widget->xAxis->range().lower+i*step+mp_bars->width()/2);
    ui->widget->xAxis->setTickVector(coor);

    QVector<QString> labels;
    labels << "norm" << "back" << "fore" << "right" << "left" << "unknown";

    qDebug() << "Coor:" << coor;
    qDebug() << "Labels:" << labels;
    qDebug() << "Values:" << values;

//    ui->widget->xAxis->setTickVectorLabels(labels);
//    ui->widget->rescaleAxes();
    ui->widget->replot();
}

void BarChartWindow::updateShow(const QString &text)
{
    ui->textBrowser->setText(text);
}

void BarChartWindow::info(const QString &title, const QString &text)
{
    QMessageBox::information(this, title, text, QMessageBox::Ok);
}
