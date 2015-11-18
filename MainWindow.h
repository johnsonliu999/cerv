#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void shakeFrm();

private slots:

    void timing();


    void on_MainWindow_destroyed();


    void on_calendarWidget_clicked(const QDate &date);

    void on_pushButton_4_clicked();

    /*** seat operation ***/

    void seatProcess();
    /**********************/

private:
    Ui::MainWindow *ui;
    QTimer* timer;
};

#endif // MAINWINDOW_H
