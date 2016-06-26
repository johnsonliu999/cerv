#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "cdatabase.h"

class SitLogic;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(bool wired, QWidget *parent = 0);
    ~MainWindow();

    void shakeFrm();

private slots:

    void timing();


    void on_MainWindow_destroyed();

    void on_pushButton_4_clicked();

    void seatProcess();

    void on_actionTrain_triggered();

    void on_calendarWidget_clicked(const QDate &date);

    void on_connectButton_clicked();

private:
    Ui::MainWindow *ui;
    QTimer* timer;

    SitLogic* p_sitLogic;

    const bool wired;

    CDatabase m_db;
};

#endif // MAINWINDOW_H
