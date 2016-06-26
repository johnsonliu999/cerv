#include "connectdialog.h"
#include "ui_connectdialog.h"

connectDialog::connectDialog(const QString &portName, const QList<QString> &devList, QWidget *parent) :
    QDialog(parent),
    p_reader(new CSerialReader(portName)),
    ui(new Ui::connectDialog)
{
    ui->setupUi(this);
    for (int i = 0; i < devList.length(); i++)
        ui->listWidget->addItem(new QListWidgetItem(devList[i]));
}

connectDialog::~connectDialog()
{
    delete ui;
}

#include "cserialreader.h"
#include <QMessageBox>
void connectDialog::on_okButton_clicked()
{
    try{
        p_reader->ConnectDevice(ui->listWidget->currentItem()->text());
    } catch(const QString& e)
    {
        QMessageBox::information(this, "Connect Error", e, QMessageBox::Ok);
        return;
    }

    QMessageBox::information(this, "Connect", "Connect Finished. Please refer to the device light.", QMessageBox::Ok);

    accept();
}
