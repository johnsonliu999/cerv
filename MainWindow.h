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

signals:
    void updateSitData(const QString portName);
    void updateModel();

public slots:
    void updatePlainText(const QString text);
    void info(const QString title, const QString text);

private slots:

    void timing();

    void on_MainWindow_destroyed();

    void sitProc();

    void on_actionTrain_triggered();

    void on_calendarWidget_clicked(const QDate &date);

    void on_connectButton_clicked();

    void on_startButton_clicked();

private:
    Ui::MainWindow *ui;

    QTimer* mp_timer;
    SitLogic* mp_sitLogic;
    QThread* mp_sitLogicThd;

    const bool wired;
    CDatabase m_db;
};

#endif // MAINWINDOW_H
