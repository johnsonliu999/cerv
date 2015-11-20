#ifndef COLLECTDIALOG_H
#define COLLECTDIALOG_H

#include <QDialog>

namespace Ui {
class collectDialog;
}

class collectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit collectDialog(QWidget *parent = 0);
    ~collectDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::collectDialog *ui;
};

#endif // COLLECTDIALOG_H
