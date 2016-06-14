#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>

namespace Ui {
class connectDialog;
}

class connectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit connectDialog(const QList<QString>&devList, QWidget *parent = 0);
    ~connectDialog();

private slots:
    void on_okButton_clicked();

private:
    Ui::connectDialog *ui;
};

#endif // CONNECTDIALOG_H
