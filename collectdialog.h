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
public:
    explicit collectDialog(const QString& portName, bool mb_replace, QWidget *parent = 0);
    ~collectDialog();

signals:
    void startTrain(const QString portName, const bool mb_replace);

private slots:
    void on_buttonStart_clicked();
public slots:
    void setPercent(int percent);
    void info(const QString title, const QString content);

private:
    Ui::collectDialog *ui;
    const QString& portName;
    const bool mb_replace;
    CPredictor* mp_predictor;

    QThread collectThread;
};

#endif // COLLECTDIALOG_H
