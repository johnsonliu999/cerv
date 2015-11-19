#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class LogDialog;
}

class LogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogDialog(QWidget *parent = 0);
    ~LogDialog();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::LogDialog *ui;
};

#endif // LOGDIALOG_H
