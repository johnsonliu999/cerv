#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>
#include <QAbstractButton>

#include "cdatabase.h"

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
    void on_newUserLabel_linkActivated(const QString &link);

private:
    Ui::LogDialog *ui;
    CDatabase m_db;
};

#endif // LOGDIALOG_H
