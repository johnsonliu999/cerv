#ifndef BARCHARTWINDOW_H
#define BARCHARTWINDOW_H

#include <QMainWindow>

class Loadder;

class QCPBars;

namespace Ui {
class BarChartWindow;
}

class BarChartWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BarChartWindow(QWidget *parent = 0);
    ~BarChartWindow();

private:
    void initalizeBarChart();

signals:
    void loadBarChartData(const QDate &from, const QDate &to);

public slots:
    void updateBarChart(const QVector<double> &values);
    void updateShow(const QString &text);
    void info(const QString &title, const QString &text);

private:
    Ui::BarChartWindow *ui;
    QCPBars *mp_bars;

    Loadder *mp_loadder;
};

#endif // BARCHARTWINDOW_H
