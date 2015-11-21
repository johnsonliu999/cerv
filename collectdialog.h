#ifndef COLLECTDIALOG_H
#define COLLECTDIALOG_H

#include <QDialog>
#include <QThread>

namespace Ui {
class collectDialog;
}

class collectDialog : public QDialog
{
    Q_OBJECT
    QThread collectThread;
public:
    explicit collectDialog(QWidget *parent = 0);
    ~collectDialog();

private slots:
    void on_buttonStart_clicked();
public slots:
    void setPercent(int percent);

private:
    Ui::collectDialog *ui;
};

#endif // COLLECTDIALOG_H
