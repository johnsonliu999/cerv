#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "cdatabase.h"

class SitLogic;
class FaceLogic;

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

private:
    void sitProc();

signals:
    void updateSitRes(const QString portName);
    void updateFaceRes();

public slots:
    void updateSitDisp(const QString text);
    void updateFaceDisp(const QString text);

    void info(const QString title, const QString text);

private slots:
    void sitTask();
    void cameraTask();

    void on_MainWindow_destroyed();
    void on_actionTrain_triggered();
    void on_calendarWidget_clicked(const QDate &date);
    void on_connectButton_clicked();
    void on_startButton_clicked();

private:
    Ui::MainWindow *ui;

    QTimer* mp_sitProcTimer;
    QTimer* mp_cameraTimer;

    SitLogic* mp_sitLogic;
    FaceLogic* mp_faceLogic;

    QThread* mp_sitLogicThd;
    QThread* mp_faceLogicThd;
    const bool wired;
    CDatabase* mp_db;
};

#endif // MAINWINDOW_H
