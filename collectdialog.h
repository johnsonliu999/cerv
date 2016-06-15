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
    explicit collectDialog(const QString& portName, bool b_replace, QWidget *parent = 0);
    ~collectDialog();

signals:
    void startTrain(const QString portName, const bool b_replace);
    void tryLoadModel();

private slots:
    void on_buttonStart_clicked();
public slots:
    void setPercent(int percent);
    void presentInformation(const QString title, const QString content);
    void presentCritical(const QString title, const QString content);

private:
    Ui::collectDialog *ui;
    const QString& portName;
    const bool b_replace;
};

#endif // COLLECTDIALOG_H
