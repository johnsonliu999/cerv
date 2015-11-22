#ifndef COLLECTDIALOG_H
#define COLLECTDIALOG_H

#include <QDialog>
#include <QThread>
#include "cpredictor.h"

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

signals:
    void startTrain();
    void tryLoadModel();

private slots:
    void on_buttonStart_clicked();
public slots:
    void setPercent(int percent);
    void presentInformation(const QString title, const QString content);
    void presentCritical(const QString title, const QString content);

private:
    Ui::collectDialog *ui;
};

#endif // COLLECTDIALOG_H
